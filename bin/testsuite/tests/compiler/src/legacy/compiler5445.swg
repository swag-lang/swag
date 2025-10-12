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
    @assert(other.x == 1)
    let v = Vector2{}

    var p = v - other
    /*@print(p.x)

    var p1 = v.tt(other)
    @print(p1.x)*/
}

//#[Swag.PrintBc]
#test
{
    var me1: Vector2 //{1}
    //@breakpoint()
    toto({1})
}
