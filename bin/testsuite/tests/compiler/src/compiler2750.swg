#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = -1'f32
    var y = cast(u64) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = Swag.F64.Max
    var y = cast(u64) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = -1'f64
    var y = cast(u64) x
}
