using Core.Math

#test
{
    var x: Vector2{1, 1}
    var y: Vector2{1, 2}
    z := x + y
    @assert(z.x == 2)
    @assert(z.y == 3)

    z *= 2
    @assert(z.x == 4)
    @assert(z.y == 6)

    //z /= 2
}
