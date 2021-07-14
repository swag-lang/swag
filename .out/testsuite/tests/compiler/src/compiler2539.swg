#global testerror

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
    y := x
}