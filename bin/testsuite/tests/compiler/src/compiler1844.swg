
#global testerror

//#[Swag.PrintBc]
#test
{
    var ptr: *u8
    #[Swag.Safety("", true)]
    ptr[0] = 0
}