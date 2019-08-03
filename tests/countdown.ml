/**
*  Print from @number down-to 0
*/
def countdown(number:int):void => {
    for(i:int in number to 0 by -1){ // This is a downwards loop [using negative step]
        echo(i)
    }
}

// It will print integers 50 - 0
countdown(50)
