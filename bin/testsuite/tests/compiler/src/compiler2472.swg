#global testerror "San0009"
#global testerror "Saf0009"
#[Swag.Safety("overflow", true)]
#test
{
    var x: s8 = Swag.S8.Max
    x *= 2
}
