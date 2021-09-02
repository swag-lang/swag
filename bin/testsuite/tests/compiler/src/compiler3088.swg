//#[Swag.PrintBc]
#test
{
    x := 6
    y := 8
    g := 0
    loop i: 8 ..< 6
        g += i
    @assert(g == 8 + 7)
}