#global testerror "San0009"
#[Swag.Safety("overflow", true)]
#test
{
    var x: u16 = Swag.U16.Max
    x *= 2
}
