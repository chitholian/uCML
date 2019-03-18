
%{
    #include <stdio.h>
    extern int line_number, column_number;
    extern FILE *yyin;
    extern int yylex(void);
    extern void yyerror(char const*);
    
%}

%define parse.error verbose

%precedence LOW
%precedence HIGH


%nonassoc EQ NE LT LE GT GE

%left '-' '+'
%left '*' '/' '%'

%token IF ELSE FOR IN TO BY DEF RETURN EXTERN LAMBDA EQ NE LT LE GT GE
%token ID // Type will be added during semantic analysis.
%token NUMBER

%precedence '('


%%

program: stmts
    | %empty
 
stmts: stmt
    | stmts stmt

stmt: var_decl
    | func_decl
    | extern_decl
    | expr
    | IF '(' expr ')' block
    | IF '(' expr ')' block ELSE block
    | FOR '(' ID ':' ID IN expr TO expr ')' block
    | FOR '(' ID ':' ID IN expr TO expr BY expr ')' block
    | RETURN expr 
 
block: '{' stmts '}'
    | '{' '}' 
 
var_decl: ID ':' ID 
    | ID ':' ID '=' expr 

extern_decl: EXTERN ID '(' func_decl_args ')' ':' ID 

func_decl: DEF ID '(' func_decl_args ')' ':' ID LAMBDA block
    | DEF ID '(' ')' ':' ID LAMBDA block
    | '(' func_decl_args ')' ':' ID LAMBDA block
    | '(' ')' ':' ID LAMBDA block
 
func_decl_args: var_decl
    | func_decl_args ',' var_decl

expr: ID %prec LOW
    | ID '=' expr %prec LOW
    | ID '(' ')'
    | ID '(' call_args ')'
//     | '(' call_args ')'
//     | '(' ')'
    | '(' expr ')'
    | expr op_arithematic expr %prec HIGH
    | expr op_comparison expr %prec HIGH 
    | NUMBER

call_args: expr
    | call_args ',' expr
    
op_arithematic: '+'
    | '-'
    | '*'
    | '/'
    | '%'

op_comparison: EQ
    | NE
    | LT
    | LE
    | GT
    | GE

%%

int main(int argc, char *argv[]) {
    if(argc > 1) {
        yyin = fopen(argv[1], "r");
    }
    
    int status = yyparse();
    
    fclose(yyin);
    
    return status;
}

