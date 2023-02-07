using Swag

func toto(x: moveref s32) {}

//#[Swag.PrintBc]
#test
{
    x := 0
    toto(moveref &x)
}
