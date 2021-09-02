//#[Swag.PrintBc]
#test
{
    x := 6
    y := 8
    g := 0
    loop i: x ..< y
        g += i
    @assert(g == 6 + 7)
}