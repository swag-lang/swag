#global testerror "Err0528"
func(T) toto(x: T)->s32
{
    return T
}

//#[Swag.PrintBc]
#test
{
    @assert(toto(0's32) == 0)
    @assert(toto(1's32) == 1)
    @assert(toto(2's32) == 2)
}
