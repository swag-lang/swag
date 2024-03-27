#global testerror "San0008"

//#[Swag.PrintBc]
#[Swag.Optim("", false), Swag.Safety("sanity", true)]
func toto()
{
    var a:   s32 = undefined
    var ptr: *s32 = &a
    dref ptr += 1
}
