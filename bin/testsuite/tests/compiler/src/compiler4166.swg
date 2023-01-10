#global testerror

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#[Swag.Safety("sanity", true)]
func toto()
{
    var a: s32 = ?
    var ptr: *s32 = &a
    dref ptr += 1
}