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
#ifndef UCML_CONTEXT_H
#define UCML_CONTEXT_H

#include <string>
#include <map>
#include <stack>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>


namespace ucml {
    class Scope {
    public:
        llvm::BasicBlock *block;
        llvm::Value *returnVal;
        std::map<std::string, std::pair<llvm::Type *, llvm::Value *> > symbols;
        Scope *parent;
    };

    class Context {
        std::stack<Scope *> scopes;
    public:
        llvm::Module *module;
        llvm::LLVMContext &llvmContext;

        explicit Context(llvm::LLVMContext &context);

        std::map<std::string, std::pair<llvm::Type *, llvm::Value *> > &getSymbols();

        Scope *getCurrentScope();

        llvm::BasicBlock *getCurrentBlock();

        llvm::Value *getReturnValue();

        void setReturnValue(llvm::Value *value);

        void setCurrentBlock(llvm::BasicBlock *block);

        Scope *createNewScope(llvm::BasicBlock *withBlock = nullptr);

        void closeCurrentScope();

        bool isEmpty();

        int size();
    };
}

#endif
