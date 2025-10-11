//#[Swag.PrintBc]
#test
{
    var g = 0
    for #reverse i in 6 to 8 do
        g += i
    @assert(g == 6 + 7 + 8)
}
