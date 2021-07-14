#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := 100000'f32
    y := cast(u16) x
}

#[Swag.Safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -1'f32
    y := cast(u16) x
}

#[Swag.Safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := 100000'f64
    y := cast(u16) x
}

#[Swag.Safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -1'f64
    y := cast(u16) x
}
