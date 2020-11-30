struct AA
{
    x: *AA.BB
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
