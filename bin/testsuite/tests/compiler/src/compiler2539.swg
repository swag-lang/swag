#global testerror "Err0088"

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
    var y = x
}