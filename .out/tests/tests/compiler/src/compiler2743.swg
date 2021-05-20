#global testerror

//#[Swag.printbc]
#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -1's32
    y := cast(u64) x
}