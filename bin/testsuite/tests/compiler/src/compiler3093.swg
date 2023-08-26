//#[Swag.PrintBc]
#test
{
    var g = 0
    loop,back i: -4 .. -1
    {
        //@print(i)
        g += i
    }

    @assert(g == -1 + -2 + -3 + -4)
}