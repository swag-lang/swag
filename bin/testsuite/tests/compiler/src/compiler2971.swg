enum RGB
{
    R
    G
    B
}
#test
{
    var cpt = 0
    loop e: RGB do
        cpt += 1
    @assert(cpt == 3)
}
