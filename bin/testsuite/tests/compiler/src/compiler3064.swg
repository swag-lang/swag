#global testerror "Err0027"
struct AA
{
    vv: s32 = 666
}

impl AA
{
    mtd opCount()->u64
    {
        @assert(vv == 666)
        return 0
    }

    mtd opSlice(a, b: u64)->const [..] s32
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
