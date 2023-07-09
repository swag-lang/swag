#global testerror
using Swag

func toto(x: ref s32) {}

//#[Swag.PrintBc]
#test
{
    var x = 0
    toto(moveref &x)
}
