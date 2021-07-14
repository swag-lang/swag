#global testerror

#[Swag.NoCopy]
struct AA
{
}

#test
{
    var x: AA
    var y: AA = x
}