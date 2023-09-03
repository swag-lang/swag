#global testerror "Saf0011"

#[Swag.Safety("overflow", true)]
#test
{
    var x: u8 = 0
    x += 100
    x += 200
}