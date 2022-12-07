var G: s32
var G1: s32

func toto()->const ref s32
{
    return &G
}

func toto1()->const ref s32
{
    return &G1
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    @assert(toto() == toto1())
    @assert(ref toto() != ref toto1())
    @assert(ref toto() == cast(const ref s32) &G)
    @assert(ref toto1() == cast(const ref s32) &G1)
}
