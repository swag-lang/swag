#global testerror
#[Swag.Safety("overflow", true)]
#test
{
    var x: s8 = Swag.S8.Max
    x *= 2
}
