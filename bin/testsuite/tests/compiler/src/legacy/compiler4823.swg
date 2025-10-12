#global testerror "Saf0027"
#global testerror "Saf0027"
#[Swag.Safety(.All, .Overflow, true)]
#test
{
    var x: s32 = Swag.S32.Max
    x *= 2
}
