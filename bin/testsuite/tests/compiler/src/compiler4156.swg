#global testerror "San0002"

//#[Swag.PrintBc]
#[Swag.Safety("sanity", true)]
func toto()->*s32
{
    var a = 0
    return &a
}