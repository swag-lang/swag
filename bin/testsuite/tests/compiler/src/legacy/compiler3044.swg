struct(T: const [..] typeinfo) AA {}

impl AA
{
    mtd isIn(V: typeinfo)->bool
    {
        var ms: u64 = 0
        foreach p in T do
            if p == V do
                return true
        return false
    }
}

#test
{
    var v: AA'([s32, bool])
    @assert(v.isIn(s32))
    @assert(v.isIn(bool))
    @assert(!v.isIn(f64))
}
