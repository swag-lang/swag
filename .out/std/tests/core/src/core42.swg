
using core.Math

//#[Swag.printbc]
#test
{
    var rect: Rect{0, 0, 10, 10}
    @assert(rect.right() == 10)
    rect.offset(10)
    @assert(rect.x == 10)
    @assert(rect.right() == 20)
    rect.clear()
    @assert(rect.x == 0)
    @assert(rect.height == 0)
}