%option noyywrap yylineno
%{
   #include<stdio.h>
   #include<stdlib.h>
   #include "UCaml.tab.h"
   #include<string.h>
  int yylinenu = 1;
  int yycolno = 1;
  void yyerror(const char* msg);
%}


  void handleToken(char* token);
  char * output; 

reserved def|if|else|for|in|to|return|by
modifier extern
arithmetic_operator "+"|"-"|"*"|"/"|"%"
relational_operator "<"|">"|"<="|">="|"!="|"=="
comma_operator ,
colon_operator :
lamba_operator =>
assignment_operator =
bracket "("|")"|"{"|"}"|"["|"]"
datatypes int|double|void
digit [0-9]
digits {digit}+
Float {digits}("."{digits})?
Double {digits}("."{digits})?(([Ee][+-])?{digits}("."{digits})?)?
letter [A-Za-z]
id ({letter}|"_")({letter}|{digit}|"_"|"-")*
whitespace [ \t\v\f\r]+
Built_in_func "echo"
NEW_LINE "\n"
other .
%%

{reserved} { 
              if(strcmp(yytext,"if")==0)
             {
               return IF;
             }
              else if(strcmp(yytext,"else")==0)
             {
               return ELSE;
             }
              else if(strcmp(yytext,"def")==0)
             {
               return DEF;
             }
              else if(strcmp(yytext,"by")==0)
             {
               return BY;
             }
              else if(strcmp(yytext,"to")==0)
             {
               return TO;
             }
              else if(strcmp(yytext,"in")==0)
             {
               return IN;
             }
              else if(strcmp(yytext,"return")==0)
             {
               return RETURN;
             }
              else if(strcmp(yytext,"for")==0)
             {
               return FOR;
             }
           }

{modifier} { return EXTERN;}

{datatypes} {
              if(strcmp(yytext,"int")==0)
             {
               return INT;
             }
              else if(strcmp(yytext,"double")==0)
             {
               return DOUBLE;
             }
            }
{bracket} {
              /*if(strcmp(yytext,"(")==0)
             {
               return OP;
             }
              else if(strcmp(yytext,")")==0)
             {
               return CP;
             }
              else if(strcmp(yytext,"{")==0)
             {
               return OB;
             }
              else if(strcmp(yytext,"}")==0)
             {
               return CB;
             }
              else if(strcmp(yytext,"[")==0)
             {
               return OS;
             }
              else if(strcmp(yytext,"]")==0)
             {
               return CS;
             }*/
             return yytext[0];
          }

{arithmetic_operator} {
         /*if(strcmp(yytext,"+")==0)
            {
              return ADD;
            }
         else if(strcmp(yytext,"-")==0)
            {
              return SUB;
            }
         else if(strcmp(yytext,"*")==0)
            {
              return MUL;
            }
         else if(strcmp(yytext,"/")==0)
            {
              return DIV;
            }
         else if(strcmp(yytext,"%")==0)
            {
              return MOD;
            }*/
            return yytext[0];
           }

{relational_operator} {
         if(strcmp(yytext,"<")==0)
            {
              return LT;                
            }
         else if(strcmp(yytext,">")==0)
            {
              return GT;  
            }
         else if(strcmp(yytext,"<=")==0)
            {
              return LE;  
            }
         else if(strcmp(yytext,">=")==0)
            {
              return GE;  
            }
         else if(strcmp(yytext,"!=")==0)
            {
              return NE;  

            }
         else if(strcmp(yytext,"==")==0)
            {
              return ET;  

            }
                         }

{digits} {
           yylval = atoi(yytext);
           return INTEGER;
         }

 /*{Float} {handleToken("Fractional_number");}*/

{Double} {
         yylval = atof(yytext);
           return REAL;
         }

 /*{Built_in_func} {handleToken("Built_in_function");}*/

{id}     {return ID;}

 /*"\""{letter}+"\"" {handleToken("string");}*/

{whitespace} {/* Ignorable */}

 {NEW_LINE} { yylinenu++; }

{comma_operator} {return yytext[0];}
{colon_operator} {return yytext[0];}
{lamba_operator} {return LAM;}
{assignment_operator} {return yytext[0];}

{other} {yyerror("Unknown Character");}

%%


void yyerror(const char *msg) {
     fprintf(stderr,"Error Message: \"%s\" in line number : %d, column number : %d\n", msg,yylinenu,yycolno);
   }

void handleToken(char *token) {
    printf("%25s : %-25s on line %d\n",token,yytext,yylinenu);
    yyout = fopen("lexer.output","a");
    fprintf(yyout,"%25s : %-25s on line %d\n",token,yytext,yylinenu);
    fclose(yyout);
   }
