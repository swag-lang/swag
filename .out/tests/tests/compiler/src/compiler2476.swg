#global testerror
#global testerror

#[swag.safety("overflow", true)]
#test
{
    var x: s32 = swag.S32.Max
    x = x * 2
}

#[swag.safety("overflow", true)]
#test
{
    var x: s64 = swag.S64.Max
    x = x * 2
}
