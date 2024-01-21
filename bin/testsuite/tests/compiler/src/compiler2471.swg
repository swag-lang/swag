#global testerror "San0010"
#global testerror "Saf0026"

#[Swag.Safety("overflow", true)]
#test
{
    var x: u8 = 255
    x += 1
}