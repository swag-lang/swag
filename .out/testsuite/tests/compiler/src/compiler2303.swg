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

//#[Swag.printbc]
#test
{
    var v: AA
}
