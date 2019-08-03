var:int = 1 // Global scope
echo(var) // prints 1
def func():void => {
    echo(var) // has access to parent scope, prints 1
    var:int = 2 // Function scope
    echo(var) // prints 2
    if(1){
        var:int = 3 // block scope
        echo(var) // prints 3
    }
    echo(var) // restore scope, prints 2
}
func()
echo(var) // restore scope, prints 1
