struct AA
{
    vv:  s32 = 666
}

impl AA
{
    mtd opCount()->u64
    {
        @assert(me.vv == 666)
        return 4
    }

    mtd opSlice(a, b: u64)->const [..] s32
    {
        @assert(me.vv == 666)
        @assert(a == 0)
        @assert(b == 3)
        return [1, 2, 3, 4]
    }
}

func toto()->AA
{
    return {}
}

#test
{
    const X = #run toto()
    @assert(@countof(X) == 4)
    @assert(X[0] == 1)
    @assert(X[1] == 2)
    @assert(X[2] == 3)
    @assert(X[3] == 4)
}
