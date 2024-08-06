#global testerror "Err0442"
using Swag

func toto(x: &&s32) {}

//#[Swag.PrintBc]
#test
{
    toto(moveref 0)
}
