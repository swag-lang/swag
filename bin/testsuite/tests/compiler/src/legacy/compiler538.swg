struct point1
{
    x:     s32 = 666
    y:     s32
    z:     s32 = 100
}

struct point
{
    inT:     point1
    x:       s32 = 10
    y:       s32
}

//#[Swag.PrintBc]
#test
{
    var pt: point
    @assert(pt.x == 10)
    @assert(pt.inT.x == 666)
    @assert(pt.inT.z == 100)
}
