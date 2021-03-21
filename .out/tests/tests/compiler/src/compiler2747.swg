#global testerror
#global testerror
#global testerror
#global testerror

#[swag.safety("", true)]
#test
{
    x := 512'f32
    y := cast(u8) x
}

#[swag.safety("", true)]
#test
{
    x := -1'f32
    y := cast(u8) x
}

#[swag.safety("", true)]
#test
{
    x := 512'f64
    y := cast(u8) x
}

#[swag.safety("", true)]
#test
{
    x := -1'f64
    y := cast(u8) x
}
