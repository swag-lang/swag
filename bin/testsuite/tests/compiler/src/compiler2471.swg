#global testerror "San0009"
#global testerror "Saf0009"

#[Swag.Safety("overflow", true)]
#test
{
    var x: u8 = 255
    x += 1
}