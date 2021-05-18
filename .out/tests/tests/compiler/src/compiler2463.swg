#global testerror

#[swag.safety("overflow", true)]
#test
{
    var x: s64 = swag.S64.Max
    x += 1
}