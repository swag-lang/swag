struct Vector2
{
    x, y: f32
}

impl Vector2
{
    #[Swag.Overload]
    func(op: string) opBinary(using self, other: Vector2)->Vector2
    {
        return Vector2{x + other.x, y + other.y}
    }

    #[Swag.Overload]
    func(op: string) opBinary(using self, other: f32)->Vector2
    {
        return Vector2{x * other, y * other}
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
    var tt = toto()
    @assert(tt.x == 10)
    @assert(tt.y == 15)
}