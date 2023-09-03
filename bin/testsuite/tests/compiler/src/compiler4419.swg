#global testerror "Err0621"
using Swag

func toto(x: moveref s32) {}

//#[Swag.PrintBc]
#test
{
    var x = 0
    toto(moveref x)
}
