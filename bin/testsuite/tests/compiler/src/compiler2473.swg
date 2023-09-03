#global testerror "Saf0009"
#[Swag.Safety("overflow", true)]
#test
{
    var x: u8 = Swag.U8.Max
    x *= 2
}
