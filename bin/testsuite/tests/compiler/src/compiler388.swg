//#[Swag.PrintBc]
#test
{
    var cpt  = 0
    var cpt1 = 0
    var l    = [1, 2, 3, 4]
    for l
    {
        cpt1 += l[@index]; cpt += 1; }

    @assert(cpt == 4)
    @assert(cpt1 == 10)
}
