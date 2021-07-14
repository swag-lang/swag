var G = 0

//#[Swag.PrintBc]
//#[Swag.optimizebc(0)]
func toto()->s32
{
    G = 1
    return 0
}

//#[Swag.PrintBc]
#test
{
    y := 0's32
    x := y ? toto() : 666's32
    @assert(G == 0)
}