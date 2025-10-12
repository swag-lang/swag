#global skip
#global testerror

//#[Swag.PrintBc]
#test
{
    var ptr: *u8
    #[Swag.Safety(.All, "", true)]
    ptr[0] = 0
}