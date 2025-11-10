func toto(x: const [2] s32, b: s32)
{
    @assert(x[0] == 1)
    @assert(x[1] == 2)
    @assert(b == 10)
}

//#[Swag.PrintBc]
#test
{
    toto([1, 2], 10)
}
