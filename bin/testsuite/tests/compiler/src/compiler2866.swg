#global testerror

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = 0xFFFFFFFF'u32
    let y: s32 = x
}