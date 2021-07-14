#global testerror

#[Swag.Safety("overflow", true)]
#test
{
    var x: u8 = 0
    x += 100
    x += 200
}