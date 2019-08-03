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
#ifndef UCML_TOOLS_H
#define UCML_TOOLS_H

#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include "context.hpp"
#include "nodes.hpp"

#define P(s) std::cout << s << "\n"
#define E(s) std::cerr << s << "\n"
#define W(loc, msg) std::cerr << "W:L" << loc.first_line << ":C" << loc.first_column << ":" << msg << "\n"
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

        static std::pair<llvm::Type *, llvm::Value *> *getValueOfIdentifier(Context &context, const Identifier &name);

        static bool isValidType(const std::string &typeName, bool isFunction = false);
    };
}

#endif
