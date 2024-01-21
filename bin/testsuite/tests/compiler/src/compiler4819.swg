#global testerror "San0010"
#global testerror "Saf0026"
#[Swag.Safety("overflow", true)]
#test
{
    var x: u16 = Swag.U16.Max
    x *= 2
}
