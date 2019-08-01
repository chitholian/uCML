#ifndef UCML_CONTEXT_H
#define UCML_CONTEXT_H

#include <string>
#include <map>
#include <stack>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Value.h>


namespace ucml {
    class Scope {
    public:
        llvm::BasicBlock *block;
        llvm::Value *returnVal;
        std::map<std::string, llvm::Value *> symbols;
        Scope *parent;
    };

    class Context {
        std::stack<Scope *> scopes;
    public:
        llvm::Module *module;
        llvm::LLVMContext &llvmContext;

        explicit Context(llvm::LLVMContext &context);

        std::map<std::string, llvm::Value *> &getSymbols();

        Scope *getCurrentScope();

        llvm::BasicBlock *getCurrentBlock();

        llvm::Value *getReturnValue();

        void setReturnValue(llvm::Value *value);

        void pushBlock(llvm::BasicBlock *block);

        void popBlock();

        bool isEmpty();

        int size();
    };
}

#endif
