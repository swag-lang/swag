#global testerror

//#[Swag.PrintBc]
#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -1's16
    y := cast(u64) x
}