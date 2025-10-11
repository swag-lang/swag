struct V
{
    x    = 666
}

#test
{
    var v:  V
    var fn: func||(V)
    let t:  s32 = 40

    fn = func|t|(v)
    {
        @assert(v.x == 666)
        @assert(t == 40)
    }

    v.fn()
}
