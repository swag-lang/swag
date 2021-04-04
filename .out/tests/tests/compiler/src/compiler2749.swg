#global testerror
#global testerror
#global testerror
#global testerror

#[swag.safety("", true)]
#[swag.optim("", false)]
#test
{
    x := swag.MaxF32
    y := cast(u32) x
}

#[swag.safety("", true)]
#[swag.optim("", false)]
#test
{
    x := -1'f32
    y := cast(u32) x
}

#[swag.safety("", true)]
#[swag.optim("", false)]
#test
{
    x := swag.MaxF64
    y := cast(u32) x
}

#[swag.safety("", true)]
#[swag.optim("", false)]
#test
{
    x := -1'f64
    y := cast(u32) x
}
