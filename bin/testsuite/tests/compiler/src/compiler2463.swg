#global testerror

#[Swag.Safety("over", true)]
#test
{
    var x: s64 = Swag.S64.Max
    x += 1
}