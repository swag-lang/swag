//#[Swag.PrintBc]
#test
{
    var g = 0
    for i in 6 until 8:
        g += i
    @assert(g == 6 + 7)
}