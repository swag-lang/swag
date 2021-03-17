#global testerror

#[swag.nocopy]
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