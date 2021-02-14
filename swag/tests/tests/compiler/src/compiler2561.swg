var G = 666

func toto()->*~8 s32
{
    return &G
}

//#[swag.printbc]
#test
{
    x := toto()
    @assert(:x == 666)
}