#global testerror "Saf0027"
#global testerror "Saf0027"
#[Swag.Safety(.All, .Overflow, true), Swag.Optimize(.All, false)]
#test
{
    var x: s32 = Swag.S32.Max
    x = x + 2
}
