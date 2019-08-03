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
