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
    #include <cstdlib>
    
    extern void yyerror(const char* msg);
    extern int yylex();
%}

%code requires {
    #include "nodes.hpp"
    extern ucml::Block *mainBlock;
    #define YYLTYPE_IS_DECLARED 1
}

%union {
    int                         token;
    std::string                 *string;
    ucml::Node                  *node;
    ucml::Block                 *block;
    ucml::Statement             *stmt;
    ucml::Assignment            *assign;
    ucml::Expression            *expr;
    ucml::Identifier            *id;
    ucml::VariableList          *varList;
    ucml::ExpressionList        *exprList;
    ucml::VariableDeclaration   *var_decl;
}

%define parse.error verbose

%precedence LOW

%token<string>  INTEGER DOUBLE ID
%token<token>   IF ELSE FOR IN TO BY DEF RETURN EXTERN LAMBDA EQ NE LT GT LE GE

%type<id>       id
%type<block>    program stmts block
%type<stmt>     stmt func_decl extern_decl
%type<expr>     expr numeric arithmetic comparision
%type<varList>  func_decl_args
%type<exprList> call_args
%type<var_decl> var_decl

%left EQ NE LT GT LE GE
%left '+' '-'
%left '*' '/' '%'

%precedence HIGH
%precedence '('

%locations

%start program

%%

program: stmts                                              {mainBlock = $1;}
    | %empty                                                {mainBlock = new ucml::Block();}
    ;

stmts: stmt                                                 {$$ = new ucml::Block(); $$->statements.push_back($1);}
    | stmts stmt                                            {$1->statements.push_back($2);}
    ;

stmt: var_decl                                              {$$ = $1;}
    | func_decl                                             {$$ = $1;}
    | extern_decl                                           {$$ = $1;}
    | expr %prec LOW                                        {$$ = new ucml::ExprStatement(*$1);}
    | IF '(' expr ')' block                                 {$$ = new ucml::IfCondition(@$, *$3, *$5);}
    | IF '(' expr ')' block ELSE block                      {$$ = new ucml::IfCondition(@$, *$3, *$5, $7);}
    | FOR '(' id ':' id IN expr TO expr ')' block           {$$ = new ucml::ForLoop(*$3, *$5, *$7, *$9, *$11);}
    | FOR '(' id ':' id IN expr TO expr BY expr ')' block   {$$ = new ucml::ForLoop(*$3, *$5, *$7, *$9, *$13, $11);}
    | RETURN expr %prec LOW                                 {$$ = new ucml::ReturnStatement(@$, $2);}
    ;

var_decl: id ':' id                                         {$$ = new ucml::VariableDeclaration(@$, *$3, *$1);}
    | id ':' id '=' expr %prec LOW                          {$$ = new ucml::VariableDeclaration(@$, *$3, *$1, $5);}
    ;

func_decl:  DEF id '(' ')' ':' id LAMBDA block              {$$ = new ucml::FunctionDeclaration(@$, *$6, *$2, $8);}
    | DEF id '(' func_decl_args ')' ':' id LAMBDA block     {$$ = new ucml::FunctionDeclaration(@$, *$7, *$2, $9, $4);}
    ;

extern_decl: EXTERN id '(' ')' ':' id                       {$$ = new ucml::FunctionDeclaration(@$, *$6, *$2, nullptr, nullptr, true);}
    | EXTERN id '(' func_decl_args ')' ':' id               {$$ = new ucml::FunctionDeclaration(@$, *$7, *$2, nullptr, $4, true);}
    ;

expr: id %prec LOW                                          {$$ = $1;}
    | id '=' expr %prec LOW                                 {$$ = new ucml::Assignment(@$, *$1, *$3);}
    | id '(' ')'                                            {$$ = new ucml::FunctionCall(@$, *$1);}
    | id '(' call_args ')'                                  {$$ = new ucml::FunctionCall(@$, *$1, $3);}
    | '(' expr ')'                                          {$$ = $2;}
    | numeric                                               {$$ = $1;}
    | arithmetic                                            {$$ = $1;}
    | comparision                                           {$$ = $1;}
    | '-' expr %prec HIGH                                   {$$ = new ucml::UnaryOperation(@$, '-', *$2);}
    ;

block: '{' '}'                                              {$$ = new ucml::Block();}
    | '{' stmts '}'                                         {$$ = $2;}
    ;
    
id: ID                                                      {$$ = new ucml::Identifier(@$, *$1);}

func_decl_args: var_decl                                    {$$ = new ucml::VariableList(); $$->push_back($1);}
    | func_decl_args ',' var_decl                           {$1->push_back($3);}
    ;

call_args: expr                                             {$$ = new ucml::ExpressionList(); $$->push_back($1);}
    | call_args ',' expr                                    {$1->push_back($3);}
    ;

numeric: INTEGER                                            {$$ = new ucml::Integer(atol($1->c_str()));}
    | DOUBLE                                                {$$ = new ucml::Double(atof($1->c_str()));}
    ;

arithmetic: expr '+' expr                                   {$$ = new ucml::BinaryOperation(@$, '+', *$1, *$3);}
    | expr '-' expr                                         {$$ = new ucml::BinaryOperation(@$, '-', *$1, *$3);}
    | expr '*' expr                                         {$$ = new ucml::BinaryOperation(@$, '*', *$1, *$3);}
    | expr '/' expr                                         {$$ = new ucml::BinaryOperation(@$, '/', *$1, *$3);}
    | expr '%' expr                                         {$$ = new ucml::BinaryOperation(@$, '%', *$1, *$3);}
    ;

comparision: expr EQ expr                                   {$$ = new ucml::BinaryOperation(@$, EQ, *$1, *$3);}
    | expr NE expr                                          {$$ = new ucml::BinaryOperation(@$, NE, *$1, *$3);}
    | expr LT expr                                          {$$ = new ucml::BinaryOperation(@$, LT, *$1, *$3);}
    | expr GT expr                                          {$$ = new ucml::BinaryOperation(@$, GT, *$1, *$3);}
    | expr LE expr                                          {$$ = new ucml::BinaryOperation(@$, LE, *$1, *$3);}
    | expr GE expr                                          {$$ = new ucml::BinaryOperation(@$, GE, *$1, *$3);}
    ;

%%
