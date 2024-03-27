
struct point
{
    x:     s32 = 10
    y:     s32 = 200
}

impl point
{
}

//#[Swag.PrintBc]
#test
{
    var pt: point
    @assert(pt.x == 10)
    @assert(pt.y == 200)
}
