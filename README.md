
# uCML (uhh-KaM-əl)) 
####      - a strictly typed static functional programming language. 

A baby programming language for learning principles and theories of undergrad/grad level Computer Science courses on Compilers and Priciples of Programming Languages. The compiler for the uCAML is implemented using FLEX, BISON and LLVM.

### CFG
```
program -> stmts | ϵ
		
stmts   -> stmt  | stmts stmt;

stmt    -> var_decl | func_decl | extern_decl  | expr   
        | if ( expr ) block  | if ( expr ) block else block 
        | for (  id :  id in expr to expr ) block  | for (  id :  id in expr to expr by expr ) block  
        | return expr  
        
block  -> { stmts } | { }  
 
var_decl ->  id :  id  |  id :  id = expr   

extern_decl -> extern  id ( func_decl_args ) :  id 
        | extern id ( ) : id | extern id : id

func_decl -> def  id ( func_decl_args ) :  id => block
        | def id ( ) :  id => block

func_decl_args :  var_decl | func_decl_args , var_decl  

expr ->  - expr | id = expr   |  id ( call_args )  | id ( ) | id  | expr % expr   | expr * expr  
     | expr / expr   | expr +  expr   |  expr comparison expr   | expr - expr   | ( expr )   | numeric 

numeric -> int | double  

call_args  -> expr | call_args , expr    

comparison -> == | != | < | <= | > | >=
```

### Sample Codes


#### Variable Declaration 
```ts
 x:int 
 y:double = 1.0
```

#### Single Statement
```ts
 x:int = a * 5 + 5 / 5 + (100 * 7)
```

### Function Declartion
```ts
def square(x: int):int =>  { return x * x }
def sumOfSquares(x: int, y: int):int => {
   return square(x) + square(y)
}
echo(sumOfSquares(4,5)) 
```

### Logical Operators
```ts
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

```ts
if(x > y) 
      { foo(x)}
   else   
      { bar(y) }
    
```

## For Loop 
    for( identifier in start to end [by step]) { statements}
    
```ts
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
To compile this program you need `flex`, `bison`, `gcc`, `make` be installed.

For `Ubuntu` and its derivatives:

```bash
sudo apt install gcc make flex bison
```

For `Fedora`, `Red Hat`, `CentOS` :

```bash
sudo dnf install flex make gcc bison || sudo yum install flex make gcc bison
```

To build, goto `src` directory and run:
```bash
make build
```

For more info run:
```bash
make help
```

