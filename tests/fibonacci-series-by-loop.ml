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
