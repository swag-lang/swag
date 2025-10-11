#global testerror "Saf0027"
#global testerror "Saf0027"
#[Swag.Safety("overflow", true), Swag.Optimize("", false)]
#test
{
    var x: s32 = Swag.S32.Min
    x = x - 2
}
