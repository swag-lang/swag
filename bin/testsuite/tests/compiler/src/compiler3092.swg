//#[Swag.PrintBc]
#test
{
    var g = 0
    var cpt = 0
    loop i: 6 ..< 8 #back
    {
        if cpt == 0 @assert(i == 7)
        if cpt == 1 @assert(i == 6)
        g += i
        cpt += 1
    }

    @assert(g == 7 + 6)
}