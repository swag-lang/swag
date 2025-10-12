#global #[Swag.Safety(.All, "overflow", true)]
//#global testerror

#[Swag.Safety(.All, "", true)]
#test
{
    var x = 1
    x >>= 32
    x >>= 32
    //    x <<= 32
    //    #semerror x <<= 32
}
