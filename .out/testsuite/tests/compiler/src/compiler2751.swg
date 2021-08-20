#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := Swag.F64.Max
    y := cast(s64) x
}
