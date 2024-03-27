struct V
{
    x = 666
}

#test
{
    var v:  V
    var fn: closure(V   )

    fn = func(v)
    {
        @assert(v.x == 666)
    }

    v.fn()
}
