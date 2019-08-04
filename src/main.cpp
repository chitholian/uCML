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
#include <iostream>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include "tools.hpp"

ucml::Block *mainBlock;

extern FILE *yyin;

extern int yyparse();

void showUsage(char *name);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "====> Error! Input file not provided.\n";
        showUsage(argv[0]);
        return 1;
    }
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            std::cerr << "====> Error! Cannot open file \"" << argv[1] << "\".\n";
            showUsage(argv[0]);
            return 2;
        }
    }
    if (yyparse()) { // non-zero means something went wrong.
        std::cout << "-----------> SYNTAX ERROR FOUND <-----------\n";
        return 4;
    }
    std::cout << "++++++++++++++> SYNTAX IS OK <++++++++++++++\n";

    llvm::LLVMContext llvmContext;
    ucml::Context context(llvmContext);
    ucml::Tools tools = ucml::Tools::initialize(mainBlock, context);
    tools.createBuiltInFunctions();
    std::cout << "====> Generating Intermediate Representation (IR)...\n";
    llvm::Function *function = tools.generateCode();
    std::cout << "====> IR generation completed, dumping now...\n";
    if (argc > 2) {
        std::error_code errorCode;
        llvm::raw_fd_ostream fileStream(argv[2], errorCode, llvm::sys::fs::OpenFlags::F_None);
        if (errorCode.value()) {
            std::cerr << "====> Error! Cannot write to file \"" << argv[2] << "\", " << errorCode.message() << "\n";
            showUsage(argv[0]);
            return 3;
        } else {
            tools.printIR(fileStream);
            std::cout << "====> IR dumped to file \"" << argv[2] << "\", you can now use it to\n"
                                                                    "\t- run/execute the IR directly using \"lli\"\n"
                                                                    "\t- generate llvm bitcode using \"llvm-as\"\n"
                                                                    "\t- generate assembly-code using \"llc\"\n"
                                                                    "  [!] provided that the tools mentioned above are installed in your machine.\n";
        }
    } else {
        tools.printIR(llvm::outs());
    }
    tools.runCode(function);
    return 0;
}

void showUsage(char *name) {
    std::cerr << "Usage: \n     " << name << " [<source-file.ml> [<out-file.ir>]]\n";
}
