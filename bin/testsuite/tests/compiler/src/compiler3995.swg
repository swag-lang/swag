struct V
{
    x, y: f32
}

impl V
{
    mtd opEquals(other: Self)->bool
    {
        return x == other.x and y == other.y
    }
}

#test
{
    var a: V{1, 1}
    var b: V{1, 1}
    @assert(a == b)
}