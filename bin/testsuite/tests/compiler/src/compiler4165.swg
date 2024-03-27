#global testerror "San0006"

//#[Swag.PrintBc]
#[Swag.Optim("", false), Swag.Safety("sanity", true)]
func toto()
{
    var ptr: *s32
    dref ptr += 1
}
