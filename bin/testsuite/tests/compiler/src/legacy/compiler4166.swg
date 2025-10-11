#global testerror "Saf0034"

//#[Swag.PrintBc]
#[Swag.Optimize("", false), Swag.Sanity(true)]
func toto()
{
    var a:   s32 = undefined
    var ptr: *s32 = &a
    dref ptr += 1
}
