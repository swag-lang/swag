#global testerror
#global testerror
#global testerror
#global testerror

#[swag.safety("of", true)]
#test
{
    var x: s32 = swag.S32.Min
    x = x - 2
}

#[swag.safety("of", true)]
#test
{
    var x: s64 = swag.S64.Min
    x = x - 2
}

#[swag.safety("of", true)]
#test
{
    var x: s32 = swag.S32.Max
    x = x + 2
}

#[swag.safety("of", true)]
#test
{
    var x: s64 = swag.S64.Max
    x = x + 2
}
