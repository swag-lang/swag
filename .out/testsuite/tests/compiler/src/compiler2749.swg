#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := Swag.F32.Max
    y := cast(u32) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := -1'f32
    y := cast(u32) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := Swag.F64.Max
    y := cast(u32) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    x := -1'f64
    y := cast(u32) x
}
