#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.safety("overflow", true)]
#test
{
    var x: s8 = Swag.S8.Max
    x *= 2
}

#[Swag.safety("overflow", true)]
#test
{
    var x: s16 = Swag.S16.Max
    x *= 2
}

#[Swag.safety("overflow", true)]
#test
{
    var x: s32 = Swag.S32.Max
    x *= 2
}

#[Swag.safety("overflow", true)]
#test
{
    var x: s64 = Swag.S64.Max
    x *= 2
}
