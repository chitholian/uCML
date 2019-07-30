#include <vector>
#include <iostream>
#include "tools.hpp"
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

namespace ucml {

    Tools::Tools(Context &context, Block *codeBlock) : context(context), codeBlock(codeBlock) {}

    Tools Tools::initialize(Block *codeBlock, Context &context) {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();
        return Tools(context, codeBlock);
    }

    void Tools::createBuiltInFunctions() {
        std::cout << "====> Creating built-in function \"echo(int)\" ...\n";
        std::vector<llvm::Type *> arg_types;
        arg_types.push_back(llvm::Type::getInt8PtrTy(context.llvmContext));
        llvm::FunctionType *functionType = llvm::FunctionType::get(llvm::Type::getInt32Ty(context.llvmContext),
                                                                   arg_types, true);
        llvm::Function *function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage,
                                                          llvm::Twine("printf"), context.module);
        function->setCallingConv(llvm::CallingConv::C);

        arg_types = std::vector<llvm::Type *>();
        arg_types.push_back(llvm::Type::getInt32Ty(context.llvmContext));
        functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(context.llvmContext), arg_types, false);
        llvm::Function *echoFunction = llvm::Function::Create(functionType, llvm::Function::InternalLinkage,
                                                              llvm::Twine("echo"),
                                                              context.module);

        llvm::BasicBlock *block = llvm::BasicBlock::Create(context.llvmContext, "entry", echoFunction);
        context.pushBlock(block);
        const char *formatSpecifier = "%d\n";
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
        context.popBlock();

        std::cout << "====> Built-in function is created.\n";
    }

    llvm::Function *Tools::generateCode() {
        std::cout << "====> Generating Code...\n";

        std::vector<llvm::Type *> argTypes;
        llvm::FunctionType *functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(context.llvmContext),
                                                                   makeArrayRef(argTypes), false);
        llvm::Function *mainFunction = llvm::Function::Create(functionType, llvm::GlobalValue::InternalLinkage, "main",
                                                              context.module);
        llvm::BasicBlock *block = llvm::BasicBlock::Create(context.llvmContext, "entry", mainFunction, nullptr);

        context.pushBlock(block);
        codeBlock->generateCode(context);
        llvm::ReturnInst::Create(context.llvmContext, block);
        context.popBlock();
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
}
