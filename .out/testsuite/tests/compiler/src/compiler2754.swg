#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := 255'f32
    y := cast(s8) x
}

#[Swag.Safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -255'f32
    y := cast(s8) x
}

#[Swag.Safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := 255'f64
    y := cast(s8) x
}

#[Swag.Safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -255'f64
    y := cast(s8) x
}
