#global testerror
#global testerror
#global testerror
#global testerror

#test
{
    #[swag.safety("of", true)]
    var x: s8 = swag.MaxS8
    x *= 2
}

#test
{
    #[swag.safety("of", true)]
    var x: s16 = swag.MaxS16
    x *= 2
}

#test
{
    #[swag.safety("of", true)]
    var x: s32 = swag.MaxS32
    x *= 2
}

#test
{
    #[swag.safety("of", true)]
    var x: s64 = swag.MaxS64
    x *= 2
}
