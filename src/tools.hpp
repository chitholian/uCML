#ifndef UCML_TOOLS_H
#define UCML_TOOLS_H

#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include "context.hpp"
#include "nodes.hpp"

#define P(s) std::cout << s << "\n"
#define E(s) std::cerr << s << "\n"
#define FATAL(loc, msg) std::cerr << "E:L" << loc.first_line << ":C" << loc.first_column << ":" << msg << "\n"; exit(1)

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

        static llvm::Type *typeOf(const Identifier &type, llvm::LLVMContext &llvmContext);

        static llvm::Value *getValueOfIdentifier(Context &context, const std::string &name);

        static bool isValidType(const std::string &typeName, bool isFunction=false);
    };
}

#endif
