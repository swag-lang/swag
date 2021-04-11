#global testerror

#[swag.safety("of", true)]
#test
{
    var x: s32 = swag.S32.Max
    x += 1
}