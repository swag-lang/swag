var G: s32 = 666

func toto()->const ref s32
{
    return &G
}

//#[Swag.PrintBc]
#test
{
    let x: s32 = toto()
    @assert(x == 666)
}