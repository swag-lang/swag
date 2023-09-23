#global testerror "Err0563"
using Swag

func toto(x: moveref s32) {}

//#[Swag.PrintBc]
#test
{
    toto(moveref 0)
}
