#global testerror "San0010"
#global testerror "Saf0027"
#[Swag.Safety("overflow", true)]
#test
{
    var x: s8 = Swag.S8.Max
    x *= 2
}
