func toto(x: const [..] s32)
{
    @assert(@countof(x) == 3)
    @assert(x[0] == 1)
    @assert(x[1] == 2)
    @assert(x[2] == 3)
}

#test
{
    toto(cast(const [..] s32) [1, 2, 3])
}
