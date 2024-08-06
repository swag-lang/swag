#global testerror "Err0175"
#global testerror "Err0249"

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
