#global #[Swag.Safety("over", true)]
#global testerror

#[Swag.Safety("", true)]
#test
{
    x := 1
    x >>=,safe 32
    x >>= 32
//    x <<=,safe 32
//    #semerror x <<= 32
}
