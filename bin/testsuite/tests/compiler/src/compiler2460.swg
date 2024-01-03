#global testerror "San0009"
#global testerror "Saf0009"

#[Swag.Safety("overflow", true)]
#test
{
    var x: u16 = 0xFFFF
    x += 1
}