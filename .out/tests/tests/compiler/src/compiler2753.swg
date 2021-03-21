#global testerror
#global testerror
#global testerror
#global testerror

#[swag.safety("", true)]
#test
{
    x := swag.MaxF32
    y := cast(s16) x
}

#[swag.safety("", true)]
#test
{
    x := -swag.MaxF32
    y := cast(s16) x
}

#[swag.safety("", true)]
#test
{
    x := swag.MaxF64
    y := cast(s16) x
}

#[swag.safety("", true)]
#test
{
    x := -swag.MaxF64
    y := cast(s16) x
}
