struct Vector2
{
    x, y, z: f32
}

#[Swag.printbc]
func toto(y: Vector2)
{
    var tmp: {Vector2} = @{y}
    @print(tmp.item0.x)
}

#test
{
    var v: Vector2
    v.x = 1
    v.y = 2
    toto(v)
}