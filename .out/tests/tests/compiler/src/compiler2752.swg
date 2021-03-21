#global testerror
#global testerror

#[swag.safety("", true)]
#test
{
    x := swag.MaxF32
    y := cast(s32) x
}

#[swag.safety("", true)]
#test
{
    x := -swag.MaxF32
    y := cast(s32) x
}
