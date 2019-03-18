    /**************************\
    *      ATIKUR RAHMAN       *
    *     CSE 2015 - 2016      *
    * University of Chittagong *
    \**************************/

%option noyywrap

%{
    #include <stdio.h>
    #include "parser.h"
    
    int line_number = 1, column_number = 0; // Start from first line.
%}

    /**
    * A function to show analyzer error.
    * @arg: The error message.
    */
    void yyerror(char const*);
    
    /**
    * A function to increment the column number of a line, i.e. the cursor position.
    */
    void inc_column_number(void);


    // Now make some patterns for our recognizable lexemes.

DIGIT               [0-9]
LETTER              [a-zA-Z]
ALPHANUMERIC        {LETTER}|{DIGIT}

    // Identifier: Only alphanumeric and underscore, no leading digit is allowed.
ID                  (_|{LETTER})+({ALPHANUMERIC}|_)*

    // Arithematic Operators:
OP_ARITH            "+"|"-"|"/"|"*"|"%"

    // Bracket Pairs:
BRACKET             "("|")"|"{"|"}"

    // Single-character tokens (exept >, < Operators).
SINGLE_CHAR         ","|":"|"="|{OP_ARITH}|{BRACKET}

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

if                  { inc_column_number(); return IF; }
else                { inc_column_number(); return ELSE; }
for                 { inc_column_number(); return FOR; }
in                  { inc_column_number(); return IN; }
to                  { inc_column_number(); return TO; }
by                  { inc_column_number(); return BY; }
def                 { inc_column_number(); return DEF; }
return              { inc_column_number(); return RETURN; }
extern              { inc_column_number(); return EXTERN; }
"=>"                { inc_column_number(); return LAMBDA; }
"=="                { inc_column_number(); return EQ; }
"!="                { inc_column_number(); return NE; }
"<="                { inc_column_number(); return LE; }
"<"                 { inc_column_number(); return LT; }
">="                { inc_column_number(); return GE; }
">"                 { inc_column_number(); return GT; }

{SINGLE_CHAR}       { inc_column_number(); return *yytext; }
{NUMBER}            { inc_column_number(); return NUMBER; } // TODO: set yylval.
{ID}                { inc_column_number(); return ID; } // TODO: set yylval.

{LF}                { line_number += 1; column_number = 0; } // increment line number, reset column number.
{WHITESPACE}        { inc_column_number(); }

{ANY_CHAR}          { yyerror("Unrecognized character"); exit(1); }

%%

void yyerror(char const *msg) {
    fprintf(stderr, "Error : %s in line number %d, column number %d\n", msg, line_number, column_number);
}

void inc_column_number() {
    column_number += yyleng;
}
