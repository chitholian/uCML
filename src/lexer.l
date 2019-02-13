    /**************************\
    *      ATIKUR RAHMAN       *
    *     CSE 2015 - 2016      *
    * University of Chittagong *
    \**************************/

%option noyywrap
    // Prefix; Atik, Shafkat, Foysal.
%option prefix="ASF_"

%{
    #include <stdio.h>
%}
    int line_number = 1; // Start from first line.

    /**
    * A function to do something with the recognized token. 
    * For example print the token.
    * @arg: The recognized token.
    */
    void handleToken(char*);

    /**
    * A method to show analyzer error.
    * @arg: The error message.
    */
    void yyerror(char*);


    // Now make some patterns for our recognizable lexemes.

DIGIT               [0-9]
LETTER              [a-zA-Z]
ALPHANUMERIC        {LETTER}|{DIGIT}

    // Identifier: Only alphanumeric and underscore, no leading digit is allowed.
ID                  (_|{LETTER})+({ALPHANUMERIC}|_)*
    
    // Types:
DATA_TYPE           int|double
TYPE_NAME           void|{DATA_TYPE}

    // Built-In functions:
FUN_ECHO            echo

BUILT_IN            {FUN_ECHO}

    // Reserved words:
RESERVED            if|else|for|return|extern|by|def|to


    // Relational Operators:
OP_REL              ==|!=|<|>|<=|>=

    // Arithematic Operators:
OP_ARITH            "+"|"-"|"*"|"/"|"%"

    // Assignment Operators and Separators:
OP_SEPA             =>|:|=|,

    // Bracket Pairs:
BRACKET             "("|")"|"{"|"}"|"["|"]"

    // Integer number: Allow no leading zero except for zero itself.
CONST_INT           0|([1-9]+{DIGIT}*)

    // Floating point number.
CONST_FLOAT         ({CONST_INT})?\.{CONST_INT}

    // Also allow scientific notation.
CONST_SCI           ({CONST_INT}|{CONST_FLOAT})(e|E)("-"|"+")?{CONST_INT}

NUMBER              {CONST_SCI}|{CONST_FLOAT}|{CONST_INT}

    // Extras:
LF                  "\n"
CR                  "\r"
FF                  "\f"
TAB                 "\t"
SPACE               " "
WHITESPACE          {CR}|{FF}|{TAB}|{SPACE}|{LF}
ANY_CHAR            .

%%

{RESERVED}          { handleToken("Reserved word"); }

{TYPE_NAME}         { handleToken("Data/return type"); }

{BUILT_IN}          { handleToken("Built-in function"); }

{OP_REL}            { handleToken("Relational operator"); }

{OP_ARITH}          { handleToken("Arithematic operator"); }

{OP_SEPA}           { handleToken("Assignment or separator"); }

{BRACKET}           { handleToken("Bracket"); }
    
{NUMBER}            { handleToken("Number"); }

{ID}                { handleToken("Identifier"); }

{LF}                { line_number++; }

{WHITESPACE}        { /* Just ignore them. */ }

{ANY_CHAR}          { yyerror("Unknown Character"); }

%%

/**
* This is the main function where the program starts from.
*/
int main(int argc, char **argv){

    // Check if a file is provided then input from it.
    if(argc == 2){
        yyin = fopen(argv[1], "r");
        
        // Show error if file open failed.
        if(!yyin) {
            fprintf(stderr, "Cannot open the specified file: %s, input from stdin.\n", argv[1]);
        }
    }

    // Start analyzer.
    yylex();

    // Close input file (or stdin).
    fclose(yyin);

    // return success.
    return 0;
}

void yyerror(char *msg) {
    fprintf(stderr, "Error Message: \"%s\" in line number %d, Token: \"%s\"\n", msg, line_number, yytext);
}

void handleToken(char *token) {
    printf("%25s : %-25s on line %d\n", token, yytext, line_number);
}
