#global testerror

#[Swag.Safety("over", true)]
#test
{
    var x: s16 = Swag.S16.Max
    x += 1
}