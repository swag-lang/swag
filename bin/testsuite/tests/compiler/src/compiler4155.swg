#global testerror

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#[Swag.Safety("sanity", true)]
func toto()->*s32
{
    let a = 0
    let ptr = cast(^s32) &a
    ptr += 1
    return ptr
}