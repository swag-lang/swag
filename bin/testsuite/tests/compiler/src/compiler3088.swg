//#[Swag.PrintBc]
#test
{
    var x = 6
    var y = 8
    var g = 0
    loop i: 8 ..< 6
        g += i
    @assert(g == 8 + 7)
}