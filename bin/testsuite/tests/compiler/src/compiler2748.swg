#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = 100000'f32
    var y = cast(u16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = -1'f32
    var y = cast(u16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = 100000'f64
    var y = cast(u16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = -1'f64
    var y = cast(u16) x
}
