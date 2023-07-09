//#[Swag.PrintBc]
#test
{
    var x = 6
    var y = 8
    var g = 0
    loop i: x ..< y
        g += i
    @assert(g == 6 + 7)
}