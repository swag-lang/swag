
#global testerror

//#[Swag.PrintBc]
#test
{
    var ptr: *u8
    #[Swag.safety("", true)]
    ptr[0] = 0
}