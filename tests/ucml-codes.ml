 x:int 
 y:double = 1.0
 
 x:int = a * 5 + 5 / 5 + (100 * 7)
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

if(x > y) 
      { foo(x)}
   else   
      { bar(y) }
p:int = 1
for(i:int in 1 to n) {   
    echo(p) 
    p = p +1
 } p:int = 1
for(i:int in 1 to n by 2) {   
    echo(p) 
    p = p +1
 }
 
  extern  printi(val:int):void   

x:int
x = 5+6-5/5+9
echo(x)

def do_math(a: int) : int => { 
    x:int = a * 5 + 5 / 5 + (100 * 7)
    return x
}

echo(do_math(do_math(10)))
echo(do_math(10))


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


def f(): int =>{
      x:int  = 10
     return x
 }
 

 
  extern  printi(val:int):int   
 
def printstart(n:int): void => { 
   p:int = 1
 for(i:int in 1 to n) {   
    echo(p) 
    p = p + 1
 }
   
} 

printstart(4)
 
 
 extern  printi(val:int):int   
 
 def foo(i:int): int => { printi(i) return 0}
 def bar(i:int): int => { printi(i) return 1}
 
  def baz(x:int, y:int): int => {
   if(x > y) 
      { foo(x)}
   else   
      { bar(y) }
   
   return 0
} 

baz(30,10)
