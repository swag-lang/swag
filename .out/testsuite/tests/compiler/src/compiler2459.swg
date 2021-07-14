#global testerror

#[Swag.Safety("overflow", true)]
#test
{
    var x: s8 = 0
    x += 100
    x += 100
}