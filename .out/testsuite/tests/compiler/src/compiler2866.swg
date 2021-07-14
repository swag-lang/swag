#global testerror

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := 0xFFFFFFFF'u32
    var y: s32 = x
}