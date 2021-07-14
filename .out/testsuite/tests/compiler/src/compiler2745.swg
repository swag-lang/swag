#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -1's8
    y := cast(u32) x
}

#[Swag.Safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -1's16
    y := cast(u32) x
}

#[Swag.Safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -1's32
    y := cast(u32) x
}

#[Swag.Safety("", true)]
#[Swag.optim("", false)]
#test
{
    x := -1's64
    y := cast(u32) x
}
