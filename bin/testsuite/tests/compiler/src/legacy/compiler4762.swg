//#[Swag.PrintBc]
func toto(x, y: s32)
{
    var g = 0
    for x to y
    {
        @assert(@index == 1 or @index == 2 or @index == 3)
        g += @index
    }
    @assert(g == 1 + 2 + 3)
}

#test
{
    toto(1, 3)
}
