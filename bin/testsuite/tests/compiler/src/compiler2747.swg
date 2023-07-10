#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = 512'f32
    let y = cast(u8) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -1'f32
    let y = cast(u8) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = 512'f64
    let y = cast(u8) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -1'f64
    let y = cast(u8) x
}
