//#[Swag.PrintBc]
#test
{
    var g = 0
    loop #back i: 6 to 8 do
        g += i
    @assert(g == 6 + 7 + 8)
}