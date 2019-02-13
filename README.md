
# UCML (uhh-KaM-əl)) 
####      - a strictly typed static functional programming language. 

A baby programming language for learning principles and theories of undergrad/grad level Computer Science courses on Compilers and Priciples of Programming Languages. The compiler for the uCAML is implemented using FLEX, BISON and LLVM.

### CFG
```
program -> stmts 
		
stmts   -> stmt  | stmts stmt;

stmt    -> var_decl | func_decl | extern_decl  | expr   
        | if ( expr ) block  | if ( expr ) block else block 
        | for (  id :  id in expr to expr ) block  | for (  id :  id in expr to expr by expr ) block  
        | return expr  
        
block  -> { stmts } | { }  
 
var_decl ->  id :  id  |  id :  id = expr   

extern_decl -> extern  id ( func_decl_args ) :  id 

func_decl -> def  id ( func_decl_args ) :  id => block   | (func_decl_args) :  id => block  
	 
func_decl_args :  epsilon  | var_decl    | func_decl_args , var_decl  

expr ->  id = expr   |  id ( call_args )  | (call_args )   |  id  | expr % expr   | expr * expr  
     | expr / expr   | expr +  expr   |  expr comparison expr   | expr - expr   | ( expr )   | numeric 

numeric -> int | double  

call_args  -> epsilon  | expr   | call_args , expr    

comparison -> == | != | < | <= | > | >=
```

### Sample Codes


#### Variable Declaration 
```
 x:int 
 y:double = 1.0
```

#### Single Statement
```
 x:int = a * 5 + 5 / 5 + (100 * 7)
```

### Function Declartion
```
def square(x: int):int =>  { return x * x }
def sumOfSquares(x: int, y: int):int => {
   return square(x) + square(y)
}
echo(sumOfSquares(4,5)) 
```

### Anonymous Function Declaration
```
(x: int, y:int, z:double):int => { return x * y * z }

echo((4,5,6.0) + (3,5,4.0))
```

### Logical Operators
```
def comparison_test(x: int, y: int): int => { 
     printi( x == y)
     printi( x != y)
     printi( x >= y)
     printi( x <= y)
     printi( x > y)
     printi( x < y)
     return x < y
}


echo(comparison_test(10,10)) 
```

## If-else Branching
    if(expression) { statements } else {statements}
```
if(x > y) 
      { foo(x)}
   else   
      { bar(y) }
    
```

## For Loop 
    for( identifier in start to end [by step]) { statements}
```
p:int = 1
for(i:int in 1 to n) {   
    echo(p) 
    p = p + 1
}


p:int = 1
for(i:int in 1 to n by 2) {   
    echo(p) 
    p = p + 1
}
```

## Compilation and Running
To compile this program you need `flex`, `gcc`, `make` be installed.

For `Ubuntu` and its derivatives:

```
sudo apt install gcc make flex
```

For `Fedora`, `Red Hat`, `CentOS` :

```
sudo dnf install flex make gcc || sudo yum install flex make gcc
```

To see available options run the following command from the project root directory:

```
make help
```

 
