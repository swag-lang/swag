//#[Swag.PrintBc]
struct point
{
    x:     s32
    y:     s32
    z:     s32
}

#test
{
    var pt: point
    @assert(pt.x == 0)
    pt.x = 10
    @assert(pt.x == 10)
}
