#global testerror

//#[swag.printbc]
#[swag.safety("", true)]
#test
{
    x := -1's16
    y := cast(u64) x
}