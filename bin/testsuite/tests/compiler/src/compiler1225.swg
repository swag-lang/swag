struct Vector2
{
    x:     s32
    y:     s32
}

impl Vector2
{
    //#[Swag.PrintBc]
    func normalize(const me)->Vector2
    {
        return Vector2{me.x + 1, me.y + 1}
    }
}

//#[Swag.PrintBc]
#test
{
    let pt:  Vector2{5, 100}
    let pt1: Vector2 = pt.normalize()
    @assert(pt1.x == 6)
    @assert(pt1.y == 101)
}
