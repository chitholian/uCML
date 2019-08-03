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

%option noyywrap

%{
    #include <iostream>
    #include <string>
    #include "parser.hpp"
    #include "tools.hpp"

    #define STORE yylval.string = new std::string(yytext, yyleng)
    #define TOKEN(t) return (yylval.token = t)

    #define YY_USER_ACTION update_location();

    void yyerror(const char* msg);
    void update_location();

    int yycolumn = 1;
%}

%option yylineno

ID              [_a-zA-Z]+[_0-9a-zA-Z]*
INTEGER         [0-9]+
FLOAT           {INTEGER}?\.0*{INTEGER}
SCIENTIFIC      ({INTEGER}|{FLOAT})[eE][\-\+]?{INTEGER}
DOUBLE          {FLOAT}|{SCIENTIFIC}

%s COMMENT

%%

<INITIAL>"//".*     {/* eat-up */}
<INITIAL>"/*"       {BEGIN(COMMENT);}
<COMMENT>"*/"       {BEGIN(INITIAL);}
<COMMENT>[^*\n]+    {/* eat-up */}
<COMMENT>"*"        {/* eat-up */}
[\n]+               {yycolumn = 1;}
[ \t\r]+            {/* ignore */}

if                  {TOKEN(IF);}
else                {TOKEN(ELSE);}
for                 {TOKEN(FOR);}
in                  {TOKEN(IN);}
to                  {TOKEN(TO);}
by                  {TOKEN(BY);}
def                 {TOKEN(DEF);}
return              {TOKEN(RETURN);}
extern              {TOKEN(EXTERN);}

"="                 {TOKEN('=');}

"+"                 {TOKEN('+');}
"-"                 {TOKEN('-');}
"*"                 {TOKEN('*');}
"/"                 {TOKEN('/');}
"%"                 {TOKEN('%');}

"=="                {TOKEN(EQ);}
"!="                {TOKEN(NE);}
">="                {TOKEN(GE);}
"<="                {TOKEN(LE);}
">"                 {TOKEN(GT);}
"<"                 {TOKEN(LT);}

"=>"                {TOKEN(LAMBDA);}

":"                 {TOKEN(':');}
","                 {TOKEN(',');}

"{"                 {TOKEN('{');}
"}"                 {TOKEN('}');}
"("                 {TOKEN('(');}
")"                 {TOKEN(')');}

{ID}                {STORE; return ID;}
{INTEGER}           {STORE; return INTEGER;}
{DOUBLE}            {STORE; return DOUBLE;}

.                   {FATAL(yylloc, "Unrecognized symbol \"" << yytext << "\"");}

%%

void yyerror(const char* msg){
    std::cerr << "E:L" << yylloc.first_line << ":C" << yylloc.first_column << ":" << msg;
    if(*yytext)
        std::cerr<< ", near \"" << yytext << "\"";
    std::cerr << ".\n";
}

void update_location(){
    if(yylloc.last_line < yylineno) yycolumn = 1;
    yylloc.first_line = yylloc.last_line = yylineno;
    yylloc.first_column = yycolumn; yylloc.last_column = yycolumn + (int) yyleng - 1;
    yycolumn += (int) yyleng;
}
