using Core.Math

#test
{
    let x: Vector2{1, 1}
    let y: Vector2{1, 2}
    var z  = x + y
    @assert(z.x == 2)
    @assert(z.y == 3)

    z *= 2
    @assert(z.x == 4)
    @assert(z.y == 6)

    //z /= 2
}
