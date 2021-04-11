#global testerror
#global testerror
#global testerror
#global testerror

#[swag.safety("of", true)]
#test
{
    var x: s8 = swag.S8.Max
    x *= 2
}

#[swag.safety("of", true)]
#test
{
    var x: s16 = swag.S16.Max
    x *= 2
}

#[swag.safety("of", true)]
#test
{
    var x: s32 = swag.S32.Max
    x *= 2
}

#[swag.safety("of", true)]
#test
{
    var x: s64 = swag.S64.Max
    x *= 2
}
