#global testerror
struct AA
{
    vv: s32 = 666
}

impl AA
{
    method opCount()->uint
    {
        @assert(vv == 666)
        return 0
    }

    method opSlice(a, b: uint)->const [..] s32
    {
        @assert(false)
        return @[1, 2, 3 ,4]
    }
}

func toto()->AA
{
    return @{}
}

const X = #run toto()
