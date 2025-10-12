#global #[Swag.Safety("", "overflow", true)]
//#global testerror

#test
{
    var x = 1
    x <<= 32
    x <<= 32
}
