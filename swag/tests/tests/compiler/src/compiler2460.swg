#global testerror

#[swag.safety("of", true)]
#test
{
    var x: u16 = 0xFFFF
    x += 1
}