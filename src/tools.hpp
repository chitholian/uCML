#ifndef UCML_TOOLS_H
#define UCML_TOOLS_H

#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include "context.hpp"
#include "nodes.hpp"

namespace ucml {
    class Tools {
        Context &context;
        Block *codeBlock;
    public:
        explicit Tools(Context &context, Block *codeBlock);

        static Tools initialize(Block *codeBlock, Context &context);

        void createBuiltInFunctions();

        llvm::Function *generateCode();

        void printIR(llvm::raw_ostream &oStream);

        llvm::GenericValue runCode(llvm::Function *function);

        static void showPrompt();
    };
}

#endif
