#global testerror

#[swag.safety("of", true)]
#test
{
    var x: s16 = swag.MaxS16
    x += 1
}