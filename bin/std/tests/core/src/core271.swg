using Core.Math

//#[Swag.PrintBc]
#test
{
    let x: Vector2{1, 1}
    let y: Vector2{1, 1}
    @assert(x == y)

    let x1: Vector2{1, 1}
    let y1: Vector2{10, 1}
    @assert(x1 != y1)
}
