//#global testerror "Err0053"

//#[Swag.PrintBc]
func toto(x: const ref const [..] s32)
{
    @assert(@countof(x) == 2)
    @assert(x[0] == 4)
    @assert(x[1] == 2)
}

//#[Swag.PrintBc]
#test
{
    toto([4, 2])
}