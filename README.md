
# uCML (uhh-KaM-əl)) 
####      - a strictly typed static functional programming language. 

A baby programming language for learning principles and theories of undergrad/grad level Computer Science courses on Compilers and Principles of Programming Languages. The compiler for the uCML is implemented using FLEX, BISON and LLVM.

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
     echo( x == y)
     echo( x != y)
     echo( x >= y)
     echo( x <= y)
     echo( x > y)
     echo( x < y)
     return x < y
}
echo(comparison_test(10,10)) 
```

## If-else Branching
    if(expression) { statements } else {statements}

```ts
if(x > y) {foo(x)}
else { bar(y) }
```

## For Loop 
    for( identifier in start to end [by step]) { statements}
    
```ts
n:int = 20
p:int = 1
for(i:int in 1 to n) {
    echo(p)
    p = p + 1
}

p = 1
for(j:int in 1 to n by 2) {
    echo(p)
    p = p + 1
}

p = 1
for(k:int in n to 1 by -1) {
    echo(k)
}
```

## Sample IR:
### Code

```ts
/**
* Returns Greatest Common Divisor of @a and @b
*/
def gcd(a:int, b:int):int => {
    if(a){ // Automatic boolean casting, any nonzero becomes true.
        return gcd(b % a, a)
    }
    return b
}

// Print GCD of 280 and 80; number 40 should be printed.
echo(gcd(280, 80))
```
### IR:

```
; ModuleID = 'main'
source_filename = "main"

@.ext_print_format_lld = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1
@.ext_print_format_lf = private unnamed_addr constant [5 x i8] c"%lf\0A\00", align 1

declare i32 @printf(i8*, ...)

define internal void @echoint(i64) {
entry:
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.ext_print_format_lld, i32 0, i32 0), i64 %0)
  ret void
}

define internal void @echodouble(double) {
entry:
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.ext_print_format_lf, i32 0, i32 0), double %0)
  ret void
}

define internal i64 @main() {
entry:
  %0 = call i64 @gcd(i64 280, i64 80)
  call void @echoint(i64 %0)
  ret i64 0
}

define internal i64 @gcd(i64, i64) {
entry:
  %a = alloca i64
  store i64 %0, i64* %a
  %b = alloca i64
  store i64 %1, i64* %b
  %2 = load i64, i64* %a
  %3 = icmp ne i64 %2, 0
  br i1 %3, label %then, label %otherwise

then:                                             ; preds = %entry
  %4 = load i64, i64* %b
  %5 = load i64, i64* %a
  %6 = srem i64 %4, %5
  %7 = load i64, i64* %a
  %8 = call i64 @gcd(i64 %6, i64 %7)
  ret i64 %8

otherwise:                                        ; preds = %entry
  br label %merge

merge:                                            ; preds = %otherwise
  %9 = load i64, i64* %b
  ret i64 %9
}
```

### Code

```ts
/**
*  Prints first @n fibonacci numbers.
*/
def fibonacci(n:int):void => {
    a:int = 0 b:int = 1 tmp:int // Semicolon, newline etc. not required, just keep any blank-space.
    for(i:int in 0 to n){
        echo(tmp = a) // Assign and print simultaneously.
        a = a + b
        b = tmp
    }
}

// Print first 50 fibonacci numbers.
fibonacci(50)
```

### IR:

```
; ModuleID = 'main'
source_filename = "main"

@.ext_print_format_lld = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1
@.ext_print_format_lf = private unnamed_addr constant [5 x i8] c"%lf\0A\00", align 1

declare i32 @printf(i8*, ...)

define internal void @echoint(i64) {
entry:
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.ext_print_format_lld, i32 0, i32 0), i64 %0)
  ret void
}

define internal void @echodouble(double) {
entry:
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.ext_print_format_lf, i32 0, i32 0), double %0)
  ret void
}

define internal i64 @main() {
entry:
  call void @fibonacci(i64 50)
  ret i64 0
}

define internal void @fibonacci(i64) {
entry:
  %n = alloca i64
  store i64 %0, i64* %n
  %a = alloca i64
  store i64 0, i64* %a
  %b = alloca i64
  store i64 1, i64* %b
  %tmp = alloca i64
  br label %init

init:                                             ; preds = %entry
  %i = alloca i64
  store i64 0, i64* %i
  br label %cond

cond:                                             ; preds = %progress, %init
  %1 = load i64, i64* %n
  %2 = load i64, i64* %i
  %3 = icmp sge i64 %2, 0
  %4 = icmp sle i64 %2, %1
  %5 = icmp sle i64 %2, 0
  %6 = icmp sge i64 %2, %1
  %7 = and i1 %5, %6
  %8 = and i1 %3, %4
  %9 = or i1 %8, %7
  br i1 %9, label %loop, label %after

loop:                                             ; preds = %cond
  %10 = load i64, i64* %a
  store i64 %10, i64* %tmp
  call void @echoint(i64 %10)
  %11 = load i64, i64* %a
  %12 = load i64, i64* %b
  %13 = add i64 %11, %12
  store i64 %13, i64* %a
  %14 = load i64, i64* %tmp
  store i64 %14, i64* %b
  br label %progress

progress:                                         ; preds = %loop
  %15 = load i64, i64* %i
  %16 = add i64 %15, 1
  store i64 %16, i64* %i
  br label %cond

after:                                            ; preds = %cond
  ret void
}
```

### Code

```ts
/* These external functions come from system shared libs */
extern sin(a:double):double
extern cos(a:double):double
extern tan(a:double):double

echo(sin(2.345))
echo(cos(2.345))
echo(tan(2.345))
```

### IR:

```
; ModuleID = 'main'
source_filename = "main"

@.ext_print_format_lld = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1
@.ext_print_format_lf = private unnamed_addr constant [5 x i8] c"%lf\0A\00", align 1

declare i32 @printf(i8*, ...)

define internal void @echoint(i64) {
entry:
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.ext_print_format_lld, i32 0, i32 0), i64 %0)
  ret void
}

define internal void @echodouble(double) {
entry:
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.ext_print_format_lf, i32 0, i32 0), double %0)
  ret void
}

define internal i64 @main() {
entry:
  %0 = call double @sin(double 2.345000e+00)
  call void @echodouble(double %0)
  %1 = call double @cos(double 2.345000e+00)
  call void @echodouble(double %1)
  %2 = call double @tan(double 2.345000e+00)
  call void @echodouble(double %2)
  ret i64 0
}

declare double @sin(double)

declare double @cos(double)

declare double @tan(double)
```


## Compilation and Running
To compile this program you need `flex`, `bison`, `gcc`, `g++`, `clang`, `make`, `llvm`, `zlib`, `ncurses` be installed.

For `Ubuntu` and its derivatives:

```sh
sudo apt install gcc g++ clang make flex bison llvm-dev zlib libncurses
```

For `Fedora`, `Red Hat`, `CentOS` :

```sh
sudo dnf install flex make gcc g++ clang bison llvm-devel zlib ncurses
```

To build, goto `src` directory and run:
```sh
make build
```

To test, goto `src` directory and run:
```sh
make test
```

For more info run:
```sh
make help
```

## Built and Tested on
    - Fedora 30 (KDE Plasma Spin)
        - gcc version 9.1.1 20190503 (Red Hat 9.1.1-1)
        - bison version 3.0.5
        - flex version 2.6.4
        - clang version 8.0.0 (Fedora 8.0.0-1.fc30)
        - llvm version 8.0.0
        - Linux Kernel version 5.1.20-300.fc30.x86_64
        
## Working
    - Variable declaration and initialization
    - Automatic type casting and conversion
    - User defined functions definition and call
    - External functions declaration and call
    - Print both integer and double numbers with echo(number) function call
    - If-else branching
    - For loop (upwards and downwards)
    - Variable scopes (Global, Function and Block scopes)
    - Integer and Floating point arithmetics (+, -, *, /, %)
    - Logical operations (==, !=, >=, <=, >, <)
    - Automatic boolean casting (any nonzero becomes true)

## Not Working
    - Nested/Local functions (Forbidden)
    - Default parameters for functions (Bug)
    - Anonymous functions (Not implemented yet)
    - Statements following a return statement in the same block may cause segmentation fault (Bug)
