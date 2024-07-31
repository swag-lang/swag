//#global testerror "Err0656"
struct A {x: s32}

//#[Swag.PrintBc]
func toto(x: {a:s32})
{
}

//#[Swag.PrintBc]
#test
{
    var x: A
    toto(x)
}