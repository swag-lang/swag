#global testerror

#[swag.safety("overflow", true)]
#test
{
    var x: s32 = swag.S32.Max
    x += 1
}