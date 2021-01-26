#global testerror

#[swag.safety("of", true)]
#test
{
    var x: s64 = swag.MaxS64
    x += 1
}