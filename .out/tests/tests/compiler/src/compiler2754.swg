#global testerror
#global testerror
#global testerror
#global testerror

#[swag.safety("", true)]
#test
{
    x := 255'f32
    y := cast(s8) x
}

#[swag.safety("", true)]
#test
{
    x := -255'f32
    y := cast(s8) x
}

#[swag.safety("", true)]
#test
{
    x := 255'f64
    y := cast(s8) x
}

#[swag.safety("", true)]
#test
{
    x := -255'f64
    y := cast(s8) x
}
