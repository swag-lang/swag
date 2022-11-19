struct V {x: s32;}
impl V
{
    mtdc opEquals(other: const ref Self)->bool
    {
        return x == other.x
    }
}

#test
{
    var a: V{1}
    var b: V{1}
    @assert(a == b)
}