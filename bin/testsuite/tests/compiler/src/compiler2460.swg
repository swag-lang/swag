#global testerror

#[Swag.Safety("over", true)]
#test
{
    var x: u16 = 0xFFFF
    x += 1
}