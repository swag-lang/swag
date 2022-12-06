#global testerror
struct AA
{
    vv: s32 = 666
}

impl AA
{
    mtd opCount()->uint
    {
        @assert(vv == 666)
        return 0
    }

    mtd opSlice(a, b: uint)->const [..] s32
    {
        @assert(false)
        return [1, 2, 3 ,4]
    }
}

func toto()->AA
{
    return {}
}

const X = #run toto()
