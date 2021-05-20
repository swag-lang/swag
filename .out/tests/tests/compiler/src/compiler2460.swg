#global testerror

#[Swag.safety("overflow", true)]
#test
{
    var x: u16 = 0xFFFF
    x += 1
}