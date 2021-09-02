using Core.Math

#test
{
    var x: Vector2{1, 1}
    var y: Vector2{1, 1}
    @assert(x == y)

    var x1: Vector2{1, 1}
    var y1: Vector2{10, 1}
    @assert(x1 != y1)
}
