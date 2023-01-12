#global testerror

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#[Swag.Safety("sanity", true)]
func toto()->*s32
{
    a := 0
    ptr := cast(^s32) &a
    ptr += 1
    return ptr
}