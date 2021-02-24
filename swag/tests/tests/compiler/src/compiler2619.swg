#global testerror

#[swag.nocopy]
struct AA
{
}

#test
{
    var x: AA
    var y: AA = x
}