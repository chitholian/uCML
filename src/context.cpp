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
#include "context.hpp"
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <iostream>
namespace ucml {
    Context::Context(llvm::LLVMContext &context) : llvmContext(context) {
        module = new llvm::Module("main", context);
    }

    llvm::BasicBlock *Context::getCurrentBlock() {
        return scopes.top()->block;
    }

    Scope *Context::getCurrentScope() {
        return scopes.top();
    }

    llvm::Value *Context::getReturnValue() {
        return scopes.top()->returnVal;
    }

    std::map<std::string, std::pair<llvm::Type *, llvm::Value *> > &Context::getSymbols() {
        return scopes.top()->symbols;
    }

    Scope *Context::createNewScope(llvm::BasicBlock *withBlock) {
        auto *scope = new Scope();
        scope->parent = scopes.empty() ? nullptr : scopes.top();
        scopes.push(scope);
        if (withBlock) setCurrentBlock(withBlock);
        return scope;
    }

    void Context::closeCurrentScope() {
        if (scopes.empty())
            return;
        scopes.pop();
    }

    bool Context::isEmpty() {
        return scopes.empty();
    }

    void Context::setCurrentBlock(llvm::BasicBlock *block) {
        scopes.empty() && createNewScope();
        scopes.top()->block = block;
    }

    void Context::setReturnValue(llvm::Value *value) {
        scopes.top()->returnVal = value;
    }

    int Context::size() {
        return scopes.size();
    }
}
