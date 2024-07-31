#global testerror "Err0110"

#[Swag.NoCopy]
struct AA
{
}

#test
{
    var x: AA
    var y: AA = x
}