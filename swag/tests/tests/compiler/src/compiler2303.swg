
struct AA
{
    xyz: *AA.BB
}

impl AA
{
    const CC = 0

    struct BB
    {
    }
}

//#[swag.printbc]
#test
{
    var v: AA
}
