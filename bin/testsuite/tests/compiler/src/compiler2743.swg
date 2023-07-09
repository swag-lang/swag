#global testerror

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = -1's32
    var y = cast(u64) x
}