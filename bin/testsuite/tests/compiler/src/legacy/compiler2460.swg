#global testerror "Saf0027"
#global testerror "Saf0027"

#[Swag.Safety(.All, "overflow", true)]
#test
{
    var x: u16 = 0xFFFF
    x += 1
}
