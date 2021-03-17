#global testerror

#[swag.safety("of", true)]
#test
{
    var x: u8 = 255
    x += 1
}