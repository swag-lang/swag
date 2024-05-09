struct Vector2
{
    x: s32
}

impl Vector2
{
    #[Swag.Inline]
    mtd(op: string) const opBinary(other: s32)->Vector2 => Self{x*other}
}

#[Swag.Optim("", false)]
#test
{
    var p1: Vector2{4}
    let s: s32 = 2
    let a = s * p1
    @assert(a.x == 8)
}