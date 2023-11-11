#global testerror "Err0053"

//#[Swag.PrintBc]
func toto(x: ref const [..] s32)
{
    x = [3, 4]
}

//#[Swag.PrintBc]
#test
{
    var a: [..] s32
    toto(a)
}