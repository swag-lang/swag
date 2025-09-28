#global testerror "Err0561"
using Swag

func toto(x: &s32) {}

//#[Swag.PrintBc]
#test
{
    var x = 0
    toto(moveref &x)
}
