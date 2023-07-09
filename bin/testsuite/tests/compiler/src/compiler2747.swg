#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = 512'f32
    var y = cast(u8) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = -1'f32
    var y = cast(u8) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = 512'f64
    var y = cast(u8) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = -1'f64
    var y = cast(u8) x
}
