#global testerror "Saf0027"
#global testerror "Saf0027"
#[Swag.Safety(.All, "overflow", true)]
#test
{
    var x: u8 = Swag.U8.Max
    x *= 2
}
