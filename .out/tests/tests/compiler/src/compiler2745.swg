#global testerror
#global testerror
#global testerror
#global testerror

#[swag.safety("", true)]
#[swag.optim("", false)]
#test
{
    x := -1's8
    y := cast(u32) x
}

#[swag.safety("", true)]
#[swag.optim("", false)]
#test
{
    x := -1's16
    y := cast(u32) x
}

#[swag.safety("", true)]
#[swag.optim("", false)]
#test
{
    x := -1's32
    y := cast(u32) x
}

#[swag.safety("", true)]
#[swag.optim("", false)]
#test
{
    x := -1's64
    y := cast(u32) x
}
