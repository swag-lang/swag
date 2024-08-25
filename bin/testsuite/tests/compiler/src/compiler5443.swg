struct Vector
{
    x, y: s32
}

impl Vector
{
    #[Swag.Inline]
    mtd(op: string) opBinary(other: Vector)->Vector => Self{x-other.x, y-other.y}
    //mtd tt(other: Vector)->Vector => Self{x-other.x, y-other.y}
}

#[Swag.Optimize("", false)]
#test
{
    var a = Vector{1, 1}
    var b = Vector{2, 5}
    var c = a - b
    @assert(c.x == -1)
    @assert(c.y == -4)
}