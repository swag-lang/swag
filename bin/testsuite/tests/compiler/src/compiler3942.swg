#global testerror "Err0469"
#global testerror "Err0481"

func toto(x: ref s32)
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
