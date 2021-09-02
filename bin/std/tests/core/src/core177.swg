
using Core.Math

//#[Swag.PrintBc]
#test
{
    x := Point{1, 1}
    y := x + 10
    @assert(y.x == y.y and y.y == 11)
}
