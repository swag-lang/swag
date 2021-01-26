#global testerror

#[swag.safety("of", true)]
#test
{
    var x: s32 = swag.MaxS32
    x += 1
}