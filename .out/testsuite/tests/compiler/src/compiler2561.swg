var G = 666

func toto()->*~64 s32
{
    return &G
}

//#[Swag.PrintBc]
#test
{
    x := toto()
    @assert(dref x == 666)
}