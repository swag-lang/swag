#global testerror "San0004"

//#[Swag.PrintBc]
#[Swag.Sanity(true)]
func toto()->*s32
{
    var a = 0
    return &a
}