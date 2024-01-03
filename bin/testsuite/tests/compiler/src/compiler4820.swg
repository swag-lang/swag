#global testerror "San0009"
#global testerror "Saf0009"
#[Swag.Safety("overflow", true)]
#test
{
    var x: u32 = Swag.U32.Max
    x *= 2
}
