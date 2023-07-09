//#[Swag.PrintBc]
#test
{
    var g = 0
    loop i: -1 .. -4
        g += i
    @assert(g == -1 + -2 + -3 + -4)
}