#global testerror "Err0465"
using Swag

func toto(x: &&s32)
{
}

//#[Swag.PrintBc]
#test
{
    var x: const [2] s32 = [1, 2]
    toto(moveref &x[0])
}
