#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := Swag.MaxF32
    y := cast(u32) x
}

#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -1'f32
    y := cast(u32) x
}

#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := Swag.MaxF64
    y := cast(u32) x
}

#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -1'f64
    y := cast(u32) x
}
