//#[Swag.PrintBc]
#test
{
    let a: func()->s32 = &F
    let b: s32 = a()
    @assert(b == 666)
}

func F()->s32
{
    return 666
}
