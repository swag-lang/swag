using Core.Math

//#[Swag.PrintBc]
#test
{
    var x = Point{1, 1}
    var y = x + 10
    @assert(y.x == y.y and y.y == 11)
}
