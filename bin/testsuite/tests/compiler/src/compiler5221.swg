//#global testerror "Err0644"

//#[Swag.PrintBc]
func toto(x: const & const [..] s32)
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