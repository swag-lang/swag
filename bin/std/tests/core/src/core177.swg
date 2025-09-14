using Core.Math

//#[Swag.PrintBc]
#test
{
    let x = Point{1, 1}
    let y = x + 10
    @assert(y.x == y.y and y.y == 11)
}
