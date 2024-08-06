#global testerror "Err0602"

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