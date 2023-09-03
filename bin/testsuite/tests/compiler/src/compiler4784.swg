#global testerror "Err0605"
#[Swag.Safety("", true)]
#test
{
    var x = cast(s32) (-1's32 - Swag.S32.Max)
    x = @abs(x)
}
