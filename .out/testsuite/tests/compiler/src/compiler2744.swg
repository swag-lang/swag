#global testerror

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -1's64
    y := cast(u64) x
}