#global testerror "San0004"

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#[Swag.Safety("sanity", true)]
func toto()->*s32
{
    var a = 0
    var ptr = cast(^s32) &a
    ptr += 1
    return ptr
}