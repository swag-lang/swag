#global testerror

#[Swag.Safety("overflow", true)]
#test
{
    var x: s64 = Swag.S64.Max
    x += 1
}