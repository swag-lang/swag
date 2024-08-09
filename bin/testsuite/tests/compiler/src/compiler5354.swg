#global testerror "Err0047"
struct AA
{
    vv: s32 = 666
}

impl AA
{
    mtd opCount()->u64
    {
        @assert(vv == 666)
        return 1
    }

    mtd opSlice(a, b: u64)->const [..] *s32
    {
        return null
    }
}

func toto()->AA
{
    return {}
}

const X = #run toto()
