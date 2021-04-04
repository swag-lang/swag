#global testerror

#[swag.safety("", true)]
#[swag.optim("", false)]
#test
{
    x := swag.MaxF64
    y := cast(s64) x
}
