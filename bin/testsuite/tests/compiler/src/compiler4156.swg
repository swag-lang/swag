#global testerror

//#[Swag.PrintBc]
#[Swag.Safety("sanity", true)]
func toto()->*s32
{
    let a = 0
    return &a
}