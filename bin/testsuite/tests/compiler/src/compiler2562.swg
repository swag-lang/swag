#global skip // no more relative pointers (18 july 2021)
var G = 666

func toto()->*~64 s32
{
    x := &G
    return x
}

//#[Swag.PrintBc]
#test
{
    x := toto()
    @assert(dref x == 666)
}