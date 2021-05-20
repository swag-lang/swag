#global testerror

#[Swag.safety("overflow", true)]
#test
{
    var x: s32 = Swag.S32.Max
    x += 1
}