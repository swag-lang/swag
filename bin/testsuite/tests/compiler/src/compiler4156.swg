#global testerror

//#[Swag.PrintBc]
#[Swag.Safety("analysis", true)]
func toto()->*s32
{
    a := 0
    return &a
}