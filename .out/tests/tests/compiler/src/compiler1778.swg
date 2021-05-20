
#global testerror

//#[Swag.printbc]
#[Swag.safety("", true)]
#test
{
    x := [1, 2]
    @memcpy(null, null, 0)
}