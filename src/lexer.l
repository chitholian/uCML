%option noyywrap

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

    // Disallow identifier to start with digit.
ID                  (_|{LETTER})+({DIGIT}|{LETTER}|_)*
    
    // Reserved words:  
IF                  if
ELSE                else
FOR                 for
RETURN              return
EXTERN              extern
BY                  by
DEF                 def

    // Data Types:
INT                 int
DOUBLE              double

    // Relational Operators:
REL_EQ              ==
REL_NE              !=
REL_LT              <
REL_GT              >
REL_LE              <=
REL_GE              >=

    // Arithematic Operators:
OP_ADD              "+"
OP_SUB              "-"
OP_MUL              "*"
OP_DIV              "/"
OP_MOD              "%"

    // Others:
COLON               ":"
LAMBDA              "=>"
ASSIGN              "="
L_PARAN             "("
R_PARAN             ")"
L_BRACK             "["
R_BRACK             "]"
L_BRACE             "{"
R_BRACE             "}"
COMMA               ","
    
    // Integer number: Don't allow leading zero.
CONST_INT           {DIGIT}+
    
CONST_DOUBLE        ({CONST_INT})?\.{CONST_INT}

    // Built-In functions:
FUN_ECHO            echo

    // Extras:
LF                  "\n"
CR                  "\r"
FF                  "\f"
TAB                 "\t"
SPACE               " "
WHITESPACE          {CR}|{FF}|{TAB}|{SPACE}|{LF}

ANY                 .

%%

{IF}                { handleToken("IF"); }
{ELSE}              { handleToken("ELSE"); }
{FOR}               { handleToken("FOR"); }
{RETURN}            { handleToken("RETURN"); }
{EXTERN}            { handleToken("EXTERN"); }
{BY}                { handleToken("BY"); }
{DEF}               { handleToken("DEF"); }

{INT}               { handleToken("INT"); }
{DOUBLE}            { handleToken("DOUBLE"); }

{FUN_ECHO}          { handleToken("FUN_ECHO"); }

{REL_EQ}            { handleToken("REL_EQ"); }
{REL_NE}            { handleToken("REL_NE"); }
{REL_LT}            { handleToken("REL_LT"); }
{REL_GT}            { handleToken("REL_GT"); }
{REL_LE}            { handleToken("REL_LE"); }
{REL_GE}            { handleToken("REL_GE"); }

{OP_ADD}            { handleToken("OP_ADD"); }
{OP_SUB}            { handleToken("OP_SUB"); }
{OP_MUL}            { handleToken("OP_MUL"); }
{OP_DIV}            { handleToken("OP_DIV"); }
{OP_MOD}            { handleToken("OP_MOD"); }

{COLON}             { handleToken("COLON"); }
{LAMBDA}            { handleToken("LAMBDA"); }
{ASSIGN}            { handleToken("ASSIGN"); }

{L_PARAN}           { handleToken("L_PARAN"); }
{R_PARAN}           { handleToken("R_PARAN"); }
{L_BRACK}           { handleToken("L_BRACK"); }
{R_BRACK}           { handleToken("R_BRACK"); }
{L_BRACE}           { handleToken("L_BRACE"); }
{R_BRACE}           { handleToken("R_BRACE"); }
{COMMA}             { handleToken("COMMA"); }
    
{CONST_INT}         { handleToken("CONST_INT"); }
{CONST_DOUBLE}      { handleToken("CONST_DOUBLE"); }

{ID}                { handleToken("ID"); }

{LF}                { line_number++; }

{WHITESPACE}        { /* Just ignore them. */ }

{ANY}               { yyerror("Unknown Character"); }

%%

/**
* This is the main function where the program starts from.
*/
int main(int argc, char **argv){

    // Check if a file is provided then input from it.
    if(argc == 2){
        yyin = fopen(argv[1], "r");
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
    printf("Found token %16s : %-20s on line %d\n", token, yytext, line_number);
}
