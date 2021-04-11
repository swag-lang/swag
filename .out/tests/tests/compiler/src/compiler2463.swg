#global testerror

#[swag.safety("of", true)]
#test
{
    var x: s64 = swag.S64.Max
    x += 1
}