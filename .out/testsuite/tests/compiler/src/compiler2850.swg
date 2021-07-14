#global testerror

#[Swag.Safety("", true)]
#test
{
    x := 127's8
    y := 1'u32
    x <<= y
    @print(cast(s64) x)
}
