#global testerror "Err0100"

#[Swag.NoCopy]
struct AA
{
}

#test
{
    var x: AA
    var y: AA = x
}