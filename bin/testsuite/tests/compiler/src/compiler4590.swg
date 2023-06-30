struct V
{
    x = 666
}

#test
{
    var v: V
    var fn: closure(V)
    var t: s32 = 40

    fn = closure|t|(v)
    {
        @assert(v.x == 666)
        @assert(t == 40)
    }

    v.fn()
}