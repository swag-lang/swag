#global testerror "Err0453"
using Swag

func toto(x: &&s32) {}

//#[Swag.PrintBc]
#test
{
    toto(moveref 0)
}
