#global testerror

#[Swag.Safety("", true)]
#test
{
    x := 1's8
    y := 100'u32
    x <<= y
    @print(cast(s64) x)
}
