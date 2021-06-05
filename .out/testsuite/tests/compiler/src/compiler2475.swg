#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.safety("overflow", true)]
#test
{
    var x: s32 = Swag.S32.Min
    x = x - 2
}

#[Swag.safety("overflow", true)]
#test
{
    var x: s64 = Swag.S64.Min
    x = x - 2
}

#[Swag.safety("overflow", true)]
#test
{
    var x: s32 = Swag.S32.Max
    x = x + 2
}

#[Swag.safety("overflow", true)]
#test
{
    var x: s64 = Swag.S64.Max
    x = x + 2
}
