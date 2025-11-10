var G: s32

func toto(x: const &s32)
{
    @assert(x == 0)
    G = 5
    @assert(x == 5)
}

//#[Swag.PrintBc]
#test
{
    toto(G)
}
