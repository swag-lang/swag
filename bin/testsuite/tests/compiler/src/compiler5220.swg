#global testerror "Err0561"

//#[Swag.PrintBc]
func toto(x: &const [..] s32)
{
    x = [3, 4]
}

//#[Swag.PrintBc]
#test
{
    var a: [..] s32
    toto(a)
}