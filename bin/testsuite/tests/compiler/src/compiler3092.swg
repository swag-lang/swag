//#[Swag.PrintBc]
#test
{
    var g = 0
    loop i: 8 ..< 6
        g += i
    @assert(g == 8 + 7)
}