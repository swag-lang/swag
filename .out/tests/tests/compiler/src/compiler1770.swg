//#[Swag.printbc]
func toto(x: const [2] s64)
{
    ptr := &x[0]
    @assert(ptr[0] == 10)
    @assert(ptr[1] == 20)
}

//#[Swag.printbc]
#test
{
    x := @[10's64, 20's64]
    toto(x)
}