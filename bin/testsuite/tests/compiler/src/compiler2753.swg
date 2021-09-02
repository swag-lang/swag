#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := Swag.F32.Max
    y := cast(s16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := -Swag.F32.Max
    y := cast(s16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := Swag.F64.Max
    y := cast(s16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := -Swag.F64.Max
    y := cast(s16) x
}
