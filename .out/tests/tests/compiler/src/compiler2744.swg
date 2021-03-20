#global testerror

//#[swag.printbc]
#[swag.safety("", true)]
#test
{
    x := -1's64
    y := cast(u64) x
}