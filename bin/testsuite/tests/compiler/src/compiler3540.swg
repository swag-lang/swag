struct Vector2
{
    x, y: f32
}

impl Vector2
{
    mtd(op: string) opBinary(other: Vector2)->Vector2
    {
        return Vector2{x + other.x, y + other.y}
    }
}

//#[Swag.PrintBc]
#test
{
    a := Vector2{1, 1}
    b := Vector2{2, 2}
    c := Vector2{3, 3}

    e := @{a + b + c}
    @assert(e.item0.x == 6)
}