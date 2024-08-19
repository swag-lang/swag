//#global testerror "Err0540"
struct A { x: s32; }

func toto(x: {x: s32})
{
    var a: A = x
}

//#[Swag.PrintBc]
#test
{
    toto({1's32})
}