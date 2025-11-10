#[Swag.Inline]
//#[Swag.PrintBc]
func isPrime(n: s32)->s32
{
    for i in 2 to n / 2
    {
        if !(n % i)
        {
            //@breakpoint()
            return 0
        }
    }

    return 1
}

//#[Swag.PrintBc]
#test
{
    //Time.Stopwatch.scopeMeasure()
    var numPrimes = 0
    for i in 4 to 25 //0000
    {
        numPrimes += isPrime(i)
    }
    @assert(numPrimes == 7)
}
