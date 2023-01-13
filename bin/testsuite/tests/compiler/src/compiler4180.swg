#global testerror

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#[Swag.Safety("sanity", true)]
func toto()
{
    var x: [10] s32
    @memset(&x, 0, 1'uint+@sizeof(x))
}
