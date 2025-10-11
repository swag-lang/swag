#global testerror "Saf0032"

//#[Swag.PrintBc]
#[Swag.Optimize("", false), Swag.Sanity(true)]
func toto()->*s32
{
    var a   = 0
    var ptr = cast([*] s32) &a
    ptr += 1
    return ptr
}
