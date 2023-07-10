#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = Swag.F32.Max
    let y = cast(s16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -Swag.F32.Max
    let y = cast(s16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = Swag.F64.Max
    let y = cast(s16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -Swag.F64.Max
    let y = cast(s16) x
}
