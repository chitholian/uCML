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
        std::cout << "====> Creating built-in functions ...\n";
        VariableList arguments;
        Identifier type({}, "int");
        Identifier name({}, "value");
        VariableDeclaration declaration({}, type, name);
        arguments.push_back(&declaration);

        Identifier funcType({}, "void");
        Identifier funcName({}, "printi");
//        FunctionCall call({}, funcName);

        FunctionDeclaration printIntegerDeclaration({}, funcType, funcName, nullptr, &arguments, true);
        printIntegerDeclaration.generateCode(context);

        Block bodyOfEcho;
        bodyOfEcho.statements.push_back(&printIntegerDeclaration);


        Identifier type2({}, "double");
        VariableDeclaration declaration2({}, type2, name);
        arguments.clear();
        arguments.push_back(&declaration2);

        Identifier funcType2({}, "void");
        Identifier funcName2({}, "printd");
//        FunctionCall call2({}, funcName2);

        FunctionDeclaration printDoubleDeclaration({}, funcType2, funcName2, nullptr, &arguments, true);
        printDoubleDeclaration.generateCode(context);



        /*std::vector<llvm::Type *> arg_types;
        arg_types.push_back(llvm::Type::getInt8PtrTy(context.llvmContext));
        llvm::FunctionType *functionType = llvm::FunctionType::get(llvm::Type::getInt32Ty(context.llvmContext),
                                                                   arg_types, true);
        llvm::Function *function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage,
                                                          llvm::Twine("printf"), context.module);
        function->setCallingConv(llvm::CallingConv::C);

        // For echo(int)
        arg_types = std::vector<llvm::Type *>();
        arg_types.push_back(llvm::Type::getInt64Ty(context.llvmContext));
        functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(context.llvmContext), arg_types, false);
        llvm::Function *echoFunction = llvm::Function::Create(functionType, llvm::Function::InternalLinkage,
                                                              llvm::Twine("echo"),
                                                              context.module);

        llvm::BasicBlock *block = llvm::BasicBlock::Create(context.llvmContext, "entry", echoFunction);
        context.createNewScope(block);
        const char *formatSpecifier = "%lld\n";
        llvm::Constant *format = llvm::ConstantDataArray::getString(context.llvmContext, formatSpecifier);
        auto *var = new llvm::GlobalVariable(*context.module, llvm::ArrayType::get(
                llvm::IntegerType::get(context.llvmContext, 8), strlen(formatSpecifier) + 1), true,
                                             llvm::GlobalValue::PrivateLinkage, format, ".str");

        llvm::Constant *zero = llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(context.llvmContext));

        std::vector<llvm::Constant *> indices;
        indices.push_back(zero);
        indices.push_back(zero);
        llvm::Constant *var_ref = llvm::ConstantExpr::getGetElementPtr(
                llvm::ArrayType::get(llvm::IntegerType::get(context.llvmContext, 8), strlen(formatSpecifier) + 1), var,
                indices);

        std::vector<llvm::Value *> args;
        args.push_back(var_ref);

        llvm::Value *toPrint = &*echoFunction->arg_begin();
        toPrint->setName("toPrint");
        args.push_back(toPrint);

        llvm::CallInst::Create(function, makeArrayRef(args), "", block);
        llvm::ReturnInst::Create(context.llvmContext, block);
        context.closeCurrentScope();*/

        std::cout << "====> Built-in functions are created.\n";
    }

    llvm::Function *Tools::generateCode() {
        std::vector<llvm::Type *> argTypes;
        llvm::FunctionType *functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(context.llvmContext),
                                                                   makeArrayRef(argTypes), false);
        llvm::Function *mainFunction = llvm::Function::Create(functionType, llvm::GlobalValue::InternalLinkage, "main",
                                                              context.module);
        llvm::BasicBlock *block = llvm::BasicBlock::Create(context.llvmContext, "entry", mainFunction, nullptr);

        context.createNewScope(block);
        codeBlock->generateCode(context);
        llvm::ReturnInst::Create(context.llvmContext, context.getCurrentBlock());
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
    Tools::getValueOfIdentifier(ucml::Context &context, const std::string &name) {
        if (context.isEmpty()) {
            llvm::GlobalValue *globalValue = context.module->getNamedValue(name);
            if (globalValue) {
                return new std::pair<llvm::Type *, llvm::Value *>(globalValue->getType(), globalValue);
            }
            return nullptr;
        } else if (context.getSymbols().find(name) != context.getSymbols().end()) {
            return &context.getSymbols()[name];
        } else {
            Scope *parentScope = context.getCurrentScope();
            while (parentScope->parent) {
                if (parentScope->parent->symbols.find(name) != parentScope->parent->symbols.end()) {
                    return &parentScope->parent->symbols[name];
                }
                parentScope = parentScope->parent;
            }
            return nullptr;
        }
    }

    bool Tools::isValidType(const std::string &typeName, bool isFunction) {
        if (typeName == "int" || typeName == "double") return true;
        return isFunction && typeName == "void";
    }
}
