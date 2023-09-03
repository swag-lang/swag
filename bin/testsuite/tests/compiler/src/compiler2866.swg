#global testerror "Saf0033"

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = 0xFFFFFFFF'u32
    let y: s32 = x
}