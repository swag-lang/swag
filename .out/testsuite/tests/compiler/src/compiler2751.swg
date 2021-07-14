#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := Swag.MaxF64
    y := cast(s64) x
}
