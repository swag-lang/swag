#global testerror "Err0093"
var G: s32 = 0

func toto()->const &s32
{
    return &G
}

//#[Swag.PrintBc]
#test
{
    @assert(G == 0)
    toto() = 4
    @assert(G == 4)
}
