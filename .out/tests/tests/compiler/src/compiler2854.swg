#global testerror
#global testerror
#global testerror
#global testerror

//#[swag.printbc]
#[swag.safety("", true)]
#test
{
    x := cast(s8) (-1's8 - swag.S8.Max)
    x = -x
}

#[swag.safety("", true)]
#test
{
    x := cast(s16) (-1's16 - swag.S16.Max)
    x = -x
}

#[swag.safety("", true)]
#test
{
    x := cast(s32) (-1's32 - swag.S32.Max)
    x = -x
}

#[swag.safety("", true)]
#test
{
    x := cast(s64) (-1's64 - swag.S64.Max)
    x = -x
}