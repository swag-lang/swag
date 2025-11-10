//#[Swag.PrintBc]
func toto(x, y: s32)
{
    var g   = 0
    var cpt = 0
    for #reverse x to y
    {
        if cpt == 0 do
            @assert(@index == 3)
        if cpt == 1 do
            @assert(@index == 2)
        if cpt == 2 do
            @assert(@index == 1)
        cpt += 1

        //@print(@index)
        @assert(@index == 1 or @index == 2 or @index == 3)
        g += @index
    }
    @assert(g == 1 + 2 + 3)
}

#test
{
    toto(1, 3)
}
