#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = 255'f32
    let y = cast(s8) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -255'f32
    let y = cast(s8) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = 255'f64
    let y = cast(s8) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -255'f64
    let y = cast(s8) x
}
