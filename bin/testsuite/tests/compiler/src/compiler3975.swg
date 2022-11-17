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
    @assert(kref toto() != kref toto1())
    @assert(kref toto() == &G)
    @assert(kref toto1() == &G1)
}
