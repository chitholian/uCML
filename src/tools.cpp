/*
   Copyright 2019 Atikur Rahman Chitholian

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include <vector>
#include <iostream>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include "tools.hpp"

namespace ucml {

    Tools::Tools(Context &context, Block *codeBlock) : context(context), codeBlock(codeBlock) {}

    Tools Tools::initialize(Block *codeBlock, Context &context) {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();
        return Tools(context, codeBlock);
    }

    void Tools::createBuiltInFunctions() {
        std::cout << "====> Creating built-in function \"echo(number)\" ...\n";

        /* printf() function from "C" */
        std::vector<llvm::Type *> arg_types;
        arg_types.push_back(llvm::Type::getInt8PtrTy(context.llvmContext));
        llvm::FunctionType *functionType = llvm::FunctionType::get(llvm::Type::getInt32Ty(context.llvmContext),
                                                                   arg_types, true);
        llvm::Function *function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage,
                                                          llvm::Twine("printf"), context.module);
        function->setCallingConv(llvm::CallingConv::C);

        /* Our built-in echo(int) function */
        arg_types = std::vector<llvm::Type *>();
        arg_types.push_back(llvm::Type::getInt64Ty(context.llvmContext));
        functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(context.llvmContext), arg_types, false);
        llvm::Function *echoInteger = llvm::Function::Create(functionType, llvm::Function::InternalLinkage,
                                                             llvm::Twine("echoint"), context.module);

        llvm::BasicBlock *block = llvm::BasicBlock::Create(context.llvmContext, "entry", echoInteger);
        llvm::IRBuilder<> builder(block);
        auto *format = llvm::cast<llvm::Constant>(builder.CreateGlobalStringPtr("%lld\n", ".ext_print_format_lld"));
        std::vector<llvm::Value *> args;
        args.push_back(format);
        args.push_back(&*echoInteger->arg_begin());
        builder.CreateCall(function, llvm::makeArrayRef(args), "");
        builder.CreateRetVoid();

        /* Our built-in echo(double) function */
        arg_types = std::vector<llvm::Type *>();
        arg_types.push_back(llvm::Type::getDoubleTy(context.llvmContext));
        functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(context.llvmContext), arg_types, false);
        llvm::Function *echoDouble = llvm::Function::Create(functionType, llvm::Function::InternalLinkage,
                                                            llvm::Twine("echodouble"), context.module);

        llvm::BasicBlock *block2 = llvm::BasicBlock::Create(context.llvmContext, "entry", echoDouble);
        builder.SetInsertPoint(block2);
        auto *format2 = llvm::cast<llvm::Constant>(builder.CreateGlobalStringPtr("%lf\n", ".ext_print_format_lf"));
        args.clear();
        args.push_back(format2);
        args.push_back(&*echoDouble->arg_begin());
        builder.CreateCall(function, llvm::makeArrayRef(args), "");
        builder.CreateRetVoid();
        std::cout << "====> Built-in functions are created.\n";
    }

    llvm::Function *Tools::generateCode() {
        std::vector<llvm::Type *> argTypes;
        llvm::FunctionType *functionType = llvm::FunctionType::get(llvm::Type::getInt64Ty(context.llvmContext),
                                                                   makeArrayRef(argTypes), false);
        llvm::Function *mainFunction = llvm::Function::Create(functionType, llvm::GlobalValue::InternalLinkage, "main",
                                                              context.module);
        llvm::BasicBlock *block = llvm::BasicBlock::Create(context.llvmContext, "entry", mainFunction, nullptr);

        context.createNewScope(block);
        codeBlock->generateCode(context);
        context.getCurrentBlock()->getTerminator() ||
        llvm::ReturnInst::Create(context.llvmContext,
                                 llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(context.llvmContext), 0, true),
                                 context.getCurrentBlock());
        context.closeCurrentScope();
        return mainFunction;
    }

    void Tools::printIR(llvm::raw_ostream &oStream) {
        context.module->print(oStream, nullptr);
    }

    llvm::GenericValue Tools::runCode(llvm::Function *mainFunction) {
        std::cout << "====> Running Code...\n";
        llvm::ExecutionEngine *executionEngine = llvm::EngineBuilder(
                std::unique_ptr<llvm::Module>(context.module)).create();
        executionEngine->finalizeObject();
        std::vector<llvm::GenericValue> args;
        llvm::GenericValue genericValue = executionEngine->runFunction(mainFunction, args);
        std::cout << "====> Execution completed.\n";
        return genericValue;
    }

    llvm::Type *Tools::typeOf(const ucml::Identifier &type, llvm::LLVMContext &llvmContext) {
        if (type.name == "int") {
            return llvm::Type::getInt64Ty(llvmContext);
        } else if (type.name == "double") {
            return llvm::Type::getDoubleTy(llvmContext);
        } else if (type.name == "void") {
            return llvm::Type::getVoidTy(llvmContext);
        }
        return nullptr;
    }

    std::pair<llvm::Type *, llvm::Value *> *
    Tools::getValueOfIdentifier(ucml::Context &context, const Identifier &identifier) {
        if (context.getSymbols().find(identifier.name) != context.getSymbols().end()) {
            return &context.getSymbols()[identifier.name];
        } else {
            Scope *parentScope = context.getCurrentScope();
            while (parentScope->parent) {
                if (parentScope->parent->symbols.find(identifier.name) != parentScope->parent->symbols.end()) {
                    return &parentScope->parent->symbols[identifier.name];
                }
                parentScope = parentScope->parent;
            }
        }
        llvm::GlobalValue *globalValue = context.module->getNamedValue(identifier.name);
        if (globalValue) {
            llvm::IRBuilder<> builder(context.getCurrentBlock());
            llvm::Type *type = globalValue->getValueType();
            return new std::pair<llvm::Type *, llvm::Value *>(type, globalValue);
        }
        return nullptr;
    }

    bool Tools::isValidType(const std::string &typeName, bool isFunction) {
        if (typeName == "int" || typeName == "double") return true;
        return isFunction && typeName == "void";
    }
}
