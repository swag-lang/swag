#global testerror

#[swag.safety("", true)]
#test
{
    x := swag.MaxF64
    y := cast(s64) x
}
