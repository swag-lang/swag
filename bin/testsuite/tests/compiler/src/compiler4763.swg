//#[Swag.PrintBc]
func toto(x, y: s32)
{
    var g = 0
    loop x..<y //#back
    {
        //@print(@index)
        @assert(@index == 1 or @index == 2)
        g += @index
    }
    @assert(g == 1+2)
}

#test
{
    toto(1, 3)
}