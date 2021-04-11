#global testerror

#[swag.safety("of", true)]
#test
{
    var x: s16 = swag.S16.Max
    x += 1
}