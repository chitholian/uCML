#include "context.hpp"
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

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

    std::map<std::string, llvm::Value *> &Context::getSymbols() {
        return scopes.top()->symbols;
    }

    bool Context::isEmpty() {
        return scopes.empty();
    }

    void Context::popBlock() {
        Scope *top = scopes.top();
        scopes.pop();
        delete (top);
    }

    void Context::pushBlock(llvm::BasicBlock *block) {
        auto *scope = new Scope();
        scope->parent = scopes.empty() ? nullptr : scopes.top();
        scope->block = block;
        scope->returnVal = nullptr;
        scopes.push(scope);
    }

    void Context::setReturnValue(llvm::Value *value) {
        scopes.top()->returnVal = value;
    }

    int Context::size() {
        return scopes.size();
    }
}
