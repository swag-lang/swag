struct AA
{
    x:   s32
}

impl AA
{
    mtd(T) opAffect(other: T)
    {
        if T == bool do
            me.x = 1
        elif T == s32 do
            me.x = 2
        else do
            me.x = 3
    }
}

#test
{
    var v: AA
    v = true
    @assert(v.x == 1)
    v = 0's32
    @assert(v.x == 2)
    v = 0'f64
    @assert(v.x == 3)
}
