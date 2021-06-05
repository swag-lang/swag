#global testerror

#[Swag.nocopy]
struct AA
{
}

#test
{
    var x: AA
    var y: AA = x
}