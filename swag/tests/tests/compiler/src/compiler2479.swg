//#[swag.printbc]
#[swag.safety("of", true)]
#test
{
    x := 0xFFFF
    #runerror y := cast(u8) x
    #runerror z := cast(u8) x
}