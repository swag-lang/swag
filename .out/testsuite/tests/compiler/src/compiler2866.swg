#global testerror

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := 0xFFFFFFFF'u32
    var y: s32 = x
}