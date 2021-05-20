#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := Swag.MaxF32
    y := cast(s16) x
}

#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -Swag.MaxF32
    y := cast(s16) x
}

#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := Swag.MaxF64
    y := cast(s16) x
}

#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -Swag.MaxF64
    y := cast(s16) x
}
