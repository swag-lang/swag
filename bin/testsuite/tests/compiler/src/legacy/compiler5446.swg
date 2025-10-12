struct Vector2
{
    x:   s32
}

impl Vector2
{
    #[Swag.Inline]
    mtd(op: string) const opBinary(other: Vector2)->Vector2 => Vector2{me.x - other.x}

    mtd const tt(other: Vector2)->Vector2 { return Vector2{me.x - other.x}; }
}

#[Swag.Optimize(.All, false)]
//#[Swag.PrintBc]
func toto(other: Vector2)
{
    let v = Vector2{}
    let p = v - other
    @assert(p.x == -1)
}

//#[Swag.PrintBc]
#test
{
    toto({1})
}
