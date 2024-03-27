#global testerror "Err0113"

#[Swag.NoCopy]
struct AA
{
}

struct BB
{
    x: AA
}

#test
{
    var x: BB
    var y  = x
}
