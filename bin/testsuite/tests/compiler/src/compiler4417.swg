#global testerror "Err0610"
using Swag

func toto(x: &s32) {}

//#[Swag.PrintBc]
#test
{
    var x = 0
    toto(moveref &x)
}
