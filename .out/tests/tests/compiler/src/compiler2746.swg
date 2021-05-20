#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -1's8
    y := cast(u16) x
}

#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -1's16
    y := cast(u16) x
}

#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -1's32
    y := cast(u16) x
}

#[Swag.safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -1's64
    y := cast(u16) x
}
