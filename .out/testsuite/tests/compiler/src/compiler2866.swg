#global testerror

//#[Swag.PrintBc]
#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := 0xFFFFFFFF'u32
    var y: s32 = x
}