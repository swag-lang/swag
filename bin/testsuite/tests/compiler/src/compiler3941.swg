//#global testerror

func toto(x: ref s32)
{
    @assert(x == 2)
    x = 3
}

//#[Swag.PrintBc]
#test
{
    var a = 2
    toto(&a)
    @assert(a == 3)
}
