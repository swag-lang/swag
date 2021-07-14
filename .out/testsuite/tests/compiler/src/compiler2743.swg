#global testerror

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -1's32
    y := cast(u64) x
}