
using core.math

//#[swag.printbc]
#test
{
    x := Point{1, 1}
    y := x + 10
    @assert(y.x == y.y && y.y == 11)
}
