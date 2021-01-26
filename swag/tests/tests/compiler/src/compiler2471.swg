#global testerror

#test
{
    #[swag.safety("of", true)]
    var x: u8 = 255
    x += 1
}