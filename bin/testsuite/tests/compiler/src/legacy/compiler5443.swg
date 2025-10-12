struct Vector { x, y: s32 }

impl Vector
{
    #[Swag.Inline]
    mtd(op: string) opBinary(other: Vector)->Vector => Vector{me.x - other.x, me.y - other.y}
    //mtd tt(other: Vector)->Vector => Vector{x-other.x, y-other.y}
}

#[Swag.Optimize(.All, false)]
#test
{
    var a = Vector{1, 1}
    var b = Vector{2, 5}
    let c = a - b
    @assert(c.x == -1)
    @assert(c.y == -4)
}
