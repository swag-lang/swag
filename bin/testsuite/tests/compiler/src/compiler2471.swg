#global testerror

#[Swag.Safety("overflow", true)]
#test
{
    var x: u8 = 255
    x += 1
}