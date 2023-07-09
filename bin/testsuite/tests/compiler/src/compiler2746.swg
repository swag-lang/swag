#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = -1's8
    var y = cast(u16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = -1's16
    var y = cast(u16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = -1's32
    var y = cast(u16) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = -1's64
    var y = cast(u16) x
}
