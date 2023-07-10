#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = 100000'f32
    let y = cast(u16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -1'f32
    let y = cast(u16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = 100000'f64
    let y = cast(u16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -1'f64
    let y = cast(u16) x
}
