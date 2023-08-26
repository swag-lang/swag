//#[Swag.PrintBc]
#test
{
    var g = 0
    loop i: 6 .. 8 #back
        g += i
    @assert(g == 6 + 7 + 8)
}