#global testerror

#[Swag.safety("overflow", true)]
#test
{
    var x: s8 = 0
    x += 100
    x += 100
}