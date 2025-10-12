#global testerror "Saf0018"

//#[Swag.PrintBc]
#[Swag.Optimize(.All, false), Swag.Sanity(true)]
func toto()
{
    var ptr: *s32
    dref ptr += 1
}
