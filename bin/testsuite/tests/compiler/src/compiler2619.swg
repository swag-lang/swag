#global testerror "Err0088"

#[Swag.NoCopy]
struct AA
{
}

#test
{
    var x: AA
    var y: AA = x
}