#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = Swag.F32.Max
    var y = cast(u32) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = -1'f32
    var y = cast(u32) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = Swag.F64.Max
    var y = cast(u32) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = -1'f64
    var y = cast(u32) x
}
