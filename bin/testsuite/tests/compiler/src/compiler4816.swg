#global testerror "Saf0027"
#[Swag.Safety("", true)]
#test
{
    var x = cast(s16) (-1's16 - Swag.S16.Max)
    x = -x
}
