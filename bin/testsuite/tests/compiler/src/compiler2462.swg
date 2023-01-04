#global testerror

#[Swag.Safety("over", true)]
#test
{
    var x: s32 = Swag.S32.Max
    x += 1
}