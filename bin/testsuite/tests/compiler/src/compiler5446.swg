struct Vector2
{
    x: s32
}

impl Vector2
{
    #[Swag.Inline]
    mtd(op: string) const opBinary(other: Vector2)->Vector2 => Self{x - other.x}

    mtd const tt(other: Vector2)->Vector2 { return Vector2{x - other.x}; }
}

#[Swag.Optimize("", false)]
//#[Swag.PrintBc]
func toto(other: Vector2)
{
    let v = Vector2{}
    var p = v - other
    @assert(p.x == -1)
}

//#[Swag.PrintBc]
#test
{
    toto({1})
}