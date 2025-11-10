struct Vector2
{
    x:   s32
}

impl Vector2
{
    #[Swag.Inline]
    mtd(op: string) const opBinary(other: s32)->Vector2 => Vector2{me.x * other}
}

//#[Swag.PrintAsm]
#[Swag.Optimize(.All, false)]
#test
{
    var p1: Vector2{4}
    let s:  s32 = 2
    let a   = s * p1
    @assert(a.x == 8)
}
