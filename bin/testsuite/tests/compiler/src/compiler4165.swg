#global testerror "San0006"

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#[Swag.Safety("sanity", true)]
func toto()
{
    var ptr: *s32
    dref ptr += 1
}