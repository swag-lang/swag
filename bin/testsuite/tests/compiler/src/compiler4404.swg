#global testerror "Err0716"
struct A
{
    z: s32
}

//#[Swag.PrintBc]
func toto(x: {a: s32})
{
    var v = x.x
}

//#[Swag.PrintBc]
#test
{
}
