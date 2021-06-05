#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := 255'f32
    y := cast(s8) x
}

#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -255'f32
    y := cast(s8) x
}

#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := 255'f64
    y := cast(s8) x
}

#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -255'f64
    y := cast(s8) x
}
