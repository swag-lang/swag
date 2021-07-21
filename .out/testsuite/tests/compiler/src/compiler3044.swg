struct(T: const [..] typeinfo) AA
{
}

impl AA
{
    method isIn(V: typeinfo)->bool
    {
        var ms: uint = 0
        visit p: T
            if p == V return true
        return false
    }
}

#test
{
    var v: AA'(@[s32, bool])
    @assert(v.isIn(s32))
    @assert(v.isIn(bool))
    @assert(!v.isIn(f64))
}