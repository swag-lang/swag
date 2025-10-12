#global testerror "Saf0027"
#global testerror "Saf0027"
#[Swag.Safety(.All, "overflow", true)]
#test
{
    var x: u32 = Swag.U32.Max
    x *= 2
}
