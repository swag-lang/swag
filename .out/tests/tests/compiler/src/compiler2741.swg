#global testerror

//#[swag.printbc]
#[swag.safety("", true)]
#[swag.optim("", false)]
#test
{
    x := -1's8
    y := cast(u64) x
}