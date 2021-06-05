#[Swag.nocopy]
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
    y :=,move x
}