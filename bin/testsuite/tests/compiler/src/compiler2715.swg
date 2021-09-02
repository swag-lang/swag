func toto(x: const [..] s32 = @[1, 2, 3])
{
    @assert(x[0] == 10)
    @assert(x[1] == 20)
    @assert(x[2] == 30)
    @assert(@countof(x) == 3)
}

//#[Swag.PrintBc]
#test
{
    toto(@[10, 20, 30])
}