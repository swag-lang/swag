struct Point { x, y: s32 }
//#[Swag.PrintBc]
#test
{
    var pt: Point
    let ptr = &pt
    @init(ptr, 1)(10, 20)
    @assert(pt.x == 10)
    @assert(pt.y == 20)
}
