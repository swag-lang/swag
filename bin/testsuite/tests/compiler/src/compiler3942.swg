#global testerror "Err0137"
#global testerror "Err0170"

func toto(x: &s32)
{
    @assert(x == 2)
    dref x = 3
}

//#[Swag.PrintBc]
#test
{
    var a = 2
    toto(&a)
    @assert(a == 3)
}
