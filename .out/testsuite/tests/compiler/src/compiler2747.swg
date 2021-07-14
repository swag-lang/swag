#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := 512'f32
    y := cast(u8) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := -1'f32
    y := cast(u8) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := 512'f64
    y := cast(u8) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := -1'f64
    y := cast(u8) x
}
