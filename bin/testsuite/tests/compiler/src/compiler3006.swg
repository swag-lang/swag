struct Vector2 { x, y: f32 }

impl Vector2
{
    #[Swag.Overload]
    func(op: string) opBinary(me, other: Vector2)->Vector2
    {
        return Vector2{me.x + other.x, me.y + other.y}
    }

    #[Swag.Overload]
    func(op: string) opBinary(me, other: f32)->Vector2
    {
        return Vector2{me.x * other, me.y * other}
    }
}

//#[Swag.PrintBc]
func toto()->Vector2
{
    var a: Vector2{10, 10}
    var v: Vector2{0, 1}
    return a + (v * 5.0)
}

#test
{
    let tt = toto()
    @assert(tt.x == 10)
    @assert(tt.y == 15)
}
