enum RGB { R, G, B }
#test
{
    var cpt = 0
    foreach e in RGB
    {
        switch e
        {
        case R:
            cpt += 1
        case G:
            cpt += 2
        case B:
            cpt += 4
        }
    }

    @assert(cpt == 7)
}
