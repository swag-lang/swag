struct V
{
    x    = 666
}

#test
{
    var v:  V
    var fn: func||(V)

    fn = func(v)
    {
        @assert(v.x == 666)
    }

    v.fn()
}
