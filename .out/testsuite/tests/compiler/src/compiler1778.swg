
#global testerror

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#test
{
    x := [1, 2]
    @memcpy(null, null, 0)
}