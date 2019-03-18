%{
   #include<stdio.h>
   #include<stdlib.h>
   extern FILE *yyin;
   extern FILE *yyout;
   extern int yylinenu;
   extern int yycolno;
   extern void yyerror(const char* msg);
%}

%error-verbose 
%right '='
%left '-'
%left '+'
%left '/'
%left '*'
%left '%'


%token LT GT LE GE NE ET
%token INTEGER REAL
%token ID
%token DEF IF ELSE FOR IN TO RETURN BY 
%token INT DOUBLE
%token EXTERN
%token LAM
%start stmts
%%
stmts   : stmt  
        | stmts stmt
        ;

stmt    : declaration 
        | func_call
        | expr   
        | IF '(' expr ')' block  
        | IF '(' expr ')' block ELSE block 
        | FOR '('  ID ':' datatype IN expr TO expr ')' block  
        | FOR '('  ID ':' datatype IN expr TO expr BY expr ')' block  
        | RETURN expr  
        ;

declaration: var_decl 
        | func_decl 
        | extern_decl 
        ;
        
block   : '{' stmts '}' 
        | '{' '}'  
        ;
 
var_decl :  ID ':'  datatype 
         |  ID ':'  datatype '=' expr
         ;   

extern_decl : EXTERN  ID '(' func_decl_args ')' ':'  datatype 
            ;

func_decl : DEF  ID '(' func_decl_args ')' ':'  datatype LAM block  
          | '('func_decl_args')' ':'  datatype LAM block  
          ;
func_call : ID '(' call_args ')'
          | '(' call_args ')'
	 
func_decl_args : var_decl    
               | func_decl_args ',' var_decl
               |
               ;  

expr :  ID '=' expr   
     |  ID '(' call_args ')'  
     | '('call_args ')'   
     |  ID  
     | expr '%' expr   
     | expr '*' expr  
     | expr '/' expr   
     | expr '+'  expr   
     |  expr comparison expr   
     | expr '-' expr   
     | '(' expr ')'   
     | numeric
     ; 

numeric : INTEGER 
        | REAL
        ;  

datatype : INT
         | DOUBLE
         ;


call_args  :  expr   
           | call_args ',' expr    
           |
           ;

comparison : ET
           | NE 
           | LT 
           | LE 
           | GT 
           | GE
           ;
%%
int main(int argc, char** argv)
{

  if(argc==2) {
      yyin = fopen(argv[1],"r");
      if(!yyin) {
         fprintf(stderr, "Cannot open the specified file: %s, input from stdin.\n",argv[1]);
       }
    }
  yyout = fopen("parser.output","a");
  yyparse();
  fclose(yyin);
  fclose(yyout);
  return 0;
}
