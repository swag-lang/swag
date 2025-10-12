#global #[Swag.Safety(.All, "overflow", true)]
//#global testerror

#test
{
    var x = 1
    x <<= 32
    x <<= 32
}
