//#[Swag.PrintBc]
#test
{
    var x = 6
    var y = 8
    var g = 0
    loop i: 6 until 8
        g += i
    @assert(g == 7 + 6)
}