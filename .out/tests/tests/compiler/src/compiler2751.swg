#global testerror

#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := Swag.MaxF64
    y := cast(s64) x
}
