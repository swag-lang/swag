#global testerror "Saf0027"
#global testerror "Saf0027"
#[Swag.Safety("overflow", true)]
#test
{
    var x: u16 = Swag.U16.Max
    x *= 2
}
