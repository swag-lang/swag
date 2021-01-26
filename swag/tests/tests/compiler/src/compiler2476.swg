#global testerror
#global testerror

#[swag.safety("of", true)]
#test
{
    var x: s32 = swag.MaxS32
    x = x * 2
}

#[swag.safety("of", true)]
#test
{
    var x: s64 = swag.MaxS64
    x = x * 2
}
