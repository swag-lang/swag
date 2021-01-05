
#global testerror

//#[swag.printbc]
#[swag.safety(true)]
#test
{
    x := [1, 2]
    @memcpy(null, null, 0)
}