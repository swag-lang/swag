#global testerror
#[Swag.Safety("overflow", true)]
#test
{
    var x: u16 = Swag.U16.Max
    x *= 2
}
