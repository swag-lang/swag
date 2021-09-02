#global testerror
#global testerror

#[Swag.Safety("overflow", true)]
#test
{
    var x: s32 = Swag.S32.Max
    x = x * 2
}

#[Swag.Safety("overflow", true)]
#test
{
    var x: s64 = Swag.S64.Max
    x = x * 2
}
