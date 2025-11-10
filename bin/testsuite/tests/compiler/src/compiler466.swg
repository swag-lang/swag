//#[Swag.PrintBc]
#test
{
    let a  = &F
    let b: s32 = a()
    @assert(b == 666)
}

func F()->s32
{
    return 666
}

func F1()->s32
{
    return 667
}
