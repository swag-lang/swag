#global testerror
#global testerror
#global testerror
#global testerror

#[swag.safety("", true)]
#[swag.optim("", false)]
#test
{
    x := 100000'f32
    y := cast(u16) x
}

#[swag.safety("", true)]
#[swag.optim("", false)]
#test
{
    x := -1'f32
    y := cast(u16) x
}

#[swag.safety("", true)]
#[swag.optim("", false)]
#test
{
    x := 100000'f64
    y := cast(u16) x
}

#[swag.safety("", true)]
#[swag.optim("", false)]
#test
{
    x := -1'f64
    y := cast(u16) x
}
