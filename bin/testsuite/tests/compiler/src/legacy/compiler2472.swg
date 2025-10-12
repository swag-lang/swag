#global testerror "Saf0027"
#global testerror "Saf0027"
#[Swag.Safety(.All, "overflow", true)]
#test
{
    var x: s8 = Swag.S8.Max
    x *= 2
}
