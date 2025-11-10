//#[Swag.PrintBc]
func toto(x, y: s32)
{
    var g   = 0
    var cpt = 0
    for #reverse x until y
    {
        if cpt == 0 do
            @assert(@index == 2)
        if cpt == 1 do
            @assert(@index == 1)
        cpt += 1

        //@print(@index)
        @assert(@index == 1 or @index == 2)
        g += @index
    }
    @assert(g == 1 + 2)
}

#test
{
    toto(1, 3)
}
