struct(T) A
{
}

impl A
{
    mtd opIndex(idx: u64)->T
    {
        return null
    }
}

struct(T) B
{
    using a: A'(*T)
}

#test
{
    var v1: A'(*s32)
    var v:  B's32
    var x = v[0]
}