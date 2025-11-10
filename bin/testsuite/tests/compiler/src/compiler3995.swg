struct V { x, y: f32 }

impl V
{
    mtd opEquals(other: V)->bool
    {
        return me.x == other.x and me.y == other.y
    }
}

#test
{
    var a: V{1, 1}
    var b: V{1, 1}
    @assert(a == b)
}
