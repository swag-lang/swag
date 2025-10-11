func toto(x: func||(s32, s32)->s32)->s32
{
    return x(1, 2)
}

func cmp(a, b: s32)->s32
{
    return a + b + 663
}

//#[Swag.PrintBc]
#test
{
    let v = toto(&cmp)
    @assert(v == 666)
}
