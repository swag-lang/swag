//#global testerror "eee"
enum AA : const [..] s32
{
    A = [1, 2, 3, 4]
}

//#[Swag.PrintBc]
#test
{
    const x = AA.A
    @assert(@countof(x) == 4)
    @assert(x[0] == 1)
    @assert(x[1] == 2)
    @assert(x[2] == 3)
    @assert(x[3] == 4)
}
