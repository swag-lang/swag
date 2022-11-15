var G: s32 = 666

func toto()->const &s32
{
    return &G
}

//#[Swag.PrintBc]
#test
{
    var x: s32 = toto()
    @assert(x == 666)
}