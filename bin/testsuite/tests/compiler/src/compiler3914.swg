func toto(x: const [1] s32, b: s32)
{
    @assert(x[0] == 1)
    @assert(b == 10)
}

//#[Swag.PrintBc]
#test
{
    toto([1], 10)
}
