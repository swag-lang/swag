var G: s32 = 0

func toto()->*s32
{
    return &G
}

//#[Swag.PrintBc]
#test
{
    @assert(G == 0)
    dref toto() = 4
    @assert(G == 4)
}
