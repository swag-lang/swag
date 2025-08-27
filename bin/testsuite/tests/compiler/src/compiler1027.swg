struct Point
{
    x:     s32 = 10
    y:     s32 = 20
}

//#[Swag.PrintBc]
#test
{
    var pt: Point
    pt.x = 0
    pt.y = 0
    let ptr = &pt
    @init(ptr, 1)
    @assert(pt.x == 10)
    @assert(pt.y == 20)
}
