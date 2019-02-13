%{
   #include<stdio.h>
   #include<string.h>
%}

  int line_number = 1;
  void yyerror(char* msg);
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

{reserved} { handleToken("reserved_word"); }

{modifier} { handleToken("Modifier"); }

{datatypes} {
              if(strcmp(yytext,"int")==0)
             {
               handleToken("Type_Integer");
             }
              else if(strcmp(yytext,"double")==0)
             {
               handleToken("Type_Double");
             }
              else if(strcmp(yytext,"void")==0)
             {
               handleToken("Type_void");
             }
            }
{bracket} {
              if(strcmp(yytext,"(")==0)
             {
               handleToken("Left_Parantheses");
             }
              else if(strcmp(yytext,")")==0)
             {
               handleToken("Right_Parantheses");
             }
              else if(strcmp(yytext,"{")==0)
             {
               handleToken("Left_Braces");
             }
              else if(strcmp(yytext,"}")==0)
             {
               handleToken("Right_Braces");
             }
              else if(strcmp(yytext,"[")==0)
             {
               handleToken("Left_Box_Bracket");
             }
              else if(strcmp(yytext,"]")==0)
             {
               handleToken("Right_Box_Bracket");
             }

          }

{arithmetic_operator} {
         if(strcmp(yytext,"+")==0)
            {
              handleToken("Addition_operator");
            }
         else if(strcmp(yytext,"-")==0)
            {
              handleToken("Subtraction_operator");
            }
         else if(strcmp(yytext,"*")==0)
            {
              handleToken("Multiplication_operator");
            }
         else if(strcmp(yytext,"/")==0)
            {
              handleToken("Division_operator");
            }
         else if(strcmp(yytext,"%")==0)
            {
              handleToken("Modulus_operator");
            }
           }

{relational_operator} {
         if(strcmp(yytext,"<")==0)
            {
              handleToken("TCLT");
            }
         else if(strcmp(yytext,">")==0)
            {
              handleToken("TCGT");
            }
         else if(strcmp(yytext,"<=")==0)
            {
              handleToken("TCLE");
            }
         else if(strcmp(yytext," >= ")==0)
            {
              handleToken("TCGE");
            }
         else if(strcmp(yytext,"!=")==0)
            {
              handleToken("TCNE");
            }
         else if(strcmp(yytext,"==")==0)
            {
              handleToken("TCEQ");
            }
                         }

{digits} {handleToken("integer");}

{Float} {handleToken("Fractional_number");}

{Double} {handleToken("Exponential_number");}

{Built_in_func} {handleToken("Built_in_function");}

{id}     {handleToken("identifier");}

"\""{letter}+"\"" {handleToken("string");}

{whitespace} {/* Ignorable */}

{NEW_LINE} { line_number++; }

{comma_operator} {handleToken("COMMA");}
{colon_operator} {handleToken("COLON");}
{lamba_operator} {handleToken("LAMBDA");}
{assignment_operator} {handleToken("Assignment");}

{other} {yyerror("Unknown Character");}

%%
int main(int argc, char** argv)
{

  if(argc==2) {
      yyin = fopen(argv[1],"r");
      if(!yyin) {
         fprintf(stderr, "Cannot open the specified file: %s, input from stdin.\n",argv[1]);
       }
    }
  
  yylex();
  fclose(yyin);
  return 0;
}

void yyerror(char *msg) {
     fprintf(stderr,"Error Message: \"%s\" in line number %d, Token: \"%s\"\n", msg,line_number,yytext);
   }

void handleToken(char *token) {
    printf("%25s : %-25s on line %d\n",token,yytext,line_number);
    yyout = fopen("lexer.output","a");
    fprintf(yyout,"%25s : %-25s on line %d\n",token,yytext,line_number);
    fclose(yyout);
   }
