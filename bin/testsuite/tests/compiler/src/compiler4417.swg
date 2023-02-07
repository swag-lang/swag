#global testerror
using Swag

func toto(x: ref s32) {}

//#[Swag.PrintBc]
#test
{
    x := 0
    toto(moveref &x)
}
