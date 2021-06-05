struct Vector2
{
    x, y, z: f32
}

func toto(y: Vector2)
{
    var tmp: {Vector2} = @{y}
    @assert(tmp.item0.x == 1.0)
}

#test
{
    var v: Vector2
    v.x = 1
    v.y = 2
    toto(v)
}