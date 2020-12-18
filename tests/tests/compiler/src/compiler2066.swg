var G = 0

//#[swag.printbc]
//#[swag.optimizebc(0)]
func toto()->s32
{
    G = 1
    return 0
}

//#[swag.printbc]
#test
{
    y := 0's32
    x := y ? toto() : 666's32
    @assert(G == 0)
}