#global testerror

#[Swag.safety("overflow", true)]
#test
{
    var x: s64 = Swag.S64.Max
    x += 1
}