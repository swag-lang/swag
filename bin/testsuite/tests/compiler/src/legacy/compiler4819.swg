#global testerror "Saf0027"
#global testerror "Saf0027"
#[Swag.Safety(.All, .Overflow, true)]
#test
{
    var x: u16 = Swag.U16.Max
    x *= 2
}
