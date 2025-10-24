#global testerror "Saf0027"
#global testerror "Saf0027"
#[Swag.Safety(.All, .Overflow, true), Swag.Optimize(.All, false)]
#test
{
    var x: s32 = Swag.S32.Min
    x = x - 2
}
