#global skip
#global testerror

//#[Swag.PrintBc]
#[Swag.Safety("", "", true)]
#test
{
    var x = [1, 2]
    @memcpy(null, null, 0)
}