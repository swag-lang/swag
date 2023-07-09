#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = 255'f32
    var y = cast(s8) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = -255'f32
    var y = cast(s8) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = 255'f64
    var y = cast(s8) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = -255'f64
    var y = cast(s8) x
}
