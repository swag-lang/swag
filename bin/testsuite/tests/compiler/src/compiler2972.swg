enum RGB { R; G; B; }
#test
{
    cpt := 0
    visit e: RGB
    {
        switch e
        {
        case R: cpt += 1
        case G: cpt += 2
        case B: cpt += 4
        }
    }

    @assert(cpt == 7)
}