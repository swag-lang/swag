#global testerror "Err0090"

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