enum RGB { R; G; B; }
#test
{
    var cpt = 0
    for e in RGB: cpt += 1
    @assert(cpt == 3)
}