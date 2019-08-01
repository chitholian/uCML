#include <iostream>
#include "tools.hpp"

ucml::Block *mainBlock;

extern FILE *yyin;

extern int yyparse();

void showUsage(char *name);

void showPrompt();

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
    llvm::Function *function = tools.generateCode();
    if (argc > 2) {
        std::error_code errorCode;
        llvm::raw_fd_ostream fileStream(argv[2], errorCode);
        if (errorCode.value()) {
            std::cerr << "====> Error! Cannot write to file \"" << argv[2] << "\", " << errorCode.message() << "\n";
            showUsage(argv[0]);
            return 3;
        } else {
            tools.printIR(fileStream);
            std::cout << "====> IR dumped to file \"" << argv[2] << "\"\n";
        }
    } else {
//        tools.printIR(llvm::outs());
    }
    tools.runCode(function);
    return 0;
}

void showUsage(char *name) {
    std::cerr << "Usage: \n     " << name << " [<source-file.ml> [<out-file.ll>]]\n";
}
