enum RGB { R; G; B; }
#test
{
    cpt := 0
    loop e: RGB cpt += 1
    @assert(cpt == 3)
}