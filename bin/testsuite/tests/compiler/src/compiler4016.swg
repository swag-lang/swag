#global #[Swag.Safety("overflow", true)]
//#global testerror

#[Swag.Safety("", true)]
#test
{
    x := 1
    x >>= 32
    x >>= 32
//    x <<= 32
//    #semerror x <<= 32
}
