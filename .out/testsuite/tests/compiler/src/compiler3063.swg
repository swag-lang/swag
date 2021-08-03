struct AA
{
    vv: s32 = 666
}

impl AA
{
    method opCount()->uint
    {
        @assert(vv == 666)
        return 4
    }

    method opSlice(a, b: uint)->const [..] s32
    {
        @assert(vv == 666)
        @assert(a == 0)
        @assert(b == 3)
        return @[1, 2, 3 ,4]
    }
}

func toto()->AA
{
    return @{}
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