#global testerror "Saf0027"
#[Swag.Safety("", true)]
#test
{
    var x = cast(s8) (-1's8 - Swag.S8.Max)
    x = -x
}
