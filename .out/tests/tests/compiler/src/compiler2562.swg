var G = 666

func toto()->*~64 s32
{
    x := &G
    return x
}

//#[swag.printbc]
#test
{
    x := toto()
    @assert(dref x == 666)
}