#global #[Swag.Safety("overflow", true)]
#global testerror

#test
{
    x := 1
    x <<=,over 32
    x <<= 32
}
