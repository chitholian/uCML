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

%{
    #include <iostream>
    #include <string>

    extern void yyerror(const char* msg);

    extern int yylex();
    extern FILE* yyin;
%}

%union {
    int                 token;
    std::string         *string;
}

%define parse.error verbose

%precedence LOW

%token<string>  INTEGER DOUBLE ID
%token<token>   IF ELSE FOR IN TO BY DEF RETURN EXTERN LAMBDA EQ NE LT GT LE GE

%left EQ NE LT GT LE GE
%left '+' '-'
%left '*' '/' '%'

%precedence HIGH
%precedence '('

%locations

%start program

%%

program: stmts
    | %empty
    ;

stmts: stmt
    | stmts stmt
    ;

stmt: var_decl
    | func_decl
    | extern_decl
    | expr %prec LOW
    | IF '(' expr ')' block
    | IF '(' expr ')' block ELSE block
    | FOR '(' ID ':' ID IN expr TO expr ')' block
    | FOR '(' ID ':' ID IN expr TO expr  BY expr ')' block
    | RETURN expr %prec LOW
    ;

var_decl: ID ':' ID
    | ID ':' ID '=' expr %prec LOW
    ;

func_decl:  DEF ID '(' ')' ':' ID LAMBDA block
    | DEF ID '(' func_decl_args ')' ':' ID LAMBDA block
    ;

extern_decl: EXTERN ID ':' ID
    | EXTERN ID '(' ')' ':' ID
    | EXTERN ID '(' func_decl_args ')' ':' ID
    ;

expr: ID %prec LOW
    | ID '=' expr %prec LOW
    | ID '(' ')'
    | ID '(' call_args ')'
    | '(' expr ')'
    | numeric
    | arithmatic
    | comparision
    | '-' expr %prec HIGH
    ;

block: '{' '}'
    | '{' stmts '}'
    ;

func_decl_args: var_decl
    | func_decl_args ',' var_decl
    ;

call_args: expr
    | call_args ',' expr
    ;

numeric: INTEGER
    | DOUBLE
    ;

arithmatic: expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | expr '%' expr
    ;

comparision: expr EQ expr
    | expr NE expr
    | expr LT expr
    | expr GT expr
    | expr LE expr
    | expr GE expr
    ;

%%

int main(int argc, char* argv[]){
    if(argc > 1){
        yyin = fopen(argv[1], "r");
    } else yyin = stdin;

    return yyparse();
}
