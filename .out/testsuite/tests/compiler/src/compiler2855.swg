#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("", true)]
#test
{
    x := cast(s8) (-1's8 - Swag.S8.Max)
    x = @abs(x)
}

#[Swag.Safety("", true)]
#test
{
    x := cast(s16) (-1's16 - Swag.S16.Max)
    x = @abs(x)
}

#[Swag.Safety("", true)]
#test
{
    x := cast(s32) (-1's32 - Swag.S32.Max)
    x = @abs(x)
}

#[Swag.Safety("", true)]
#test
{
    x := cast(s64) (-1's64 - Swag.S64.Max)
    x = @abs(x)
}
