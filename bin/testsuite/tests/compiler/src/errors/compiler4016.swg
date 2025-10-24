#global #[Swag.Safety(.All, .Overflow, true)]
//#global testerror

#[Swag.Safety(.All, .All, true)]
#test
{
    var x = 1
    x >>= 32
    x >>= 32
    //    x <<= 32
    //    #semerror x <<= 32
}
