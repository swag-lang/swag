func toto8(x: s8)
{
    @assert(x == -1)
}

func toto16(x: s16)
{
    @assert(x == 255)
}

func toto32(x: s32)
{
}

func toto64(x: s64)
{
}

const A = 0xFF
#test
{
    toto8(A)
    toto16(A)
    toto32(A)
    toto64(A)
}