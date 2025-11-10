struct Vector2 { x, y: f32 }

impl Vector2
{
    mtd(op: string) opBinary(other: Vector2)->Vector2
    {
        return Vector2{me.x + other.x, me.y + other.y}
    }
}

//#[Swag.PrintBc]
#test
{
    var a = Vector2{1, 1}
    var b = Vector2{2, 2}
    var c = Vector2{3, 3}

    var e = {a + b + c}
    @assert(e.item0.x == 6)
}
