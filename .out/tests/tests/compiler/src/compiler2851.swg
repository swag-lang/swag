#global testerror

#[Swag.safety("", true)]
#test
{
    x := 127's16
    y := 1'u32
    x >>= y
    @print(cast(s64) x)
}
