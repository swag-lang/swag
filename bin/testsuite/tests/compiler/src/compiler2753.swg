#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = Swag.F32.Max
    var y = cast(s16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = -Swag.F32.Max
    var y = cast(s16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = Swag.F64.Max
    var y = cast(s16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = -Swag.F64.Max
    var y = cast(s16) x
}
