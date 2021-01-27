//#[swag.printbc]
#[swag.safety("of", true)]
#test
{
    x := 10'u8
    #runerror x += 255
}