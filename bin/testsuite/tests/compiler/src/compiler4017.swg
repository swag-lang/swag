#global #[Swag.Safety("over", true)]
#global testerror

#test
{
    x := 1
    x <<=,safe 32
    x <<= 32
}
