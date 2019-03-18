
def square(x: int):int =>  { return x * x }
def sumOfSquares(x: int, y: int):int => {
   return square(x) + square(y)
}
echo(sumOfSquares(4,5))
 
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
