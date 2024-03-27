//#[Swag.PrintBc]
#test
{
    var g   = 0
    var cpt = 0
    loop,back i: 6 until 8
    {
        if cpt == 0 do
            @assert(i == 7)
        if cpt == 1 do
            @assert(i == 6)
        g += i
        cpt += 1
    }

    @assert(g == 7 + 6)
}
