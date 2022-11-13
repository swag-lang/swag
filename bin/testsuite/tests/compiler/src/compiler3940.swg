//#global testerror

func toto(x: &s32)
{
    @assert(x == 2)
}

//#[Swag.PrintBc]
#test
{
    a := 2
    toto(&a)
}
