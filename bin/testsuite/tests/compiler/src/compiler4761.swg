//#[Swag.PrintBc]
func toto(x, y: s32)
{
    var g = 0
    loop x..y
    {
        @assert(@index == 0 or @index == 1)
        g += @index
    }
    @assert(g == 1)
}

#test
{
    toto(0, 1)
}