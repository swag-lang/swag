#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := Swag.MaxF32
    y := cast(s32) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := -Swag.MaxF32
    y := cast(s32) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := Swag.MaxF64
    y := cast(s32) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := -Swag.MaxF64
    y := cast(s32) x
}
