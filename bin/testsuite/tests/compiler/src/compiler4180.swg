#global testerror "San0007"

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#[Swag.Safety("sanity", true)]
func toto()
{
    var x: [10] s32
    @memset(&x, 0, 1'u64+@sizeof(x))
}
