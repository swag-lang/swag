#global testerror "Saf0027"
#[Swag.Safety("", true)]
#test
{
    var x = cast(s32) (-1's32 - Swag.S32.Max)
    x = -x
}
