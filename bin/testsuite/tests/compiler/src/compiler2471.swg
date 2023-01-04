#global testerror

#[Swag.Safety("over", true)]
#test
{
    var x: u8 = 255
    x += 1
}