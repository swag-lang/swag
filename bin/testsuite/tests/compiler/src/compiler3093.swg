//#[Swag.PrintBc]
#test
{
    var g = 0
    loop i: -4 .. -1 #back
    {
        //@print(i)
        g += i
    }

    @assert(g == -1 + -2 + -3 + -4)
}