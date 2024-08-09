#global testerror "Err0101"

#[Swag.NoCopy]
struct AA
{
}

#test
{
    var x: AA
    var y: AA = x
}