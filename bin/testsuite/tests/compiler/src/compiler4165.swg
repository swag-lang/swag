#global testerror "San0006"

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#[Swag.Sanity(true)]
func toto()
{
    var ptr: *s32
    dref ptr += 1
}