#global testerror "Err0744"
struct BB
{
    x: bool
}

impl BB
{
    mtd opDrop()
    {
    }
}

union AA
{
    x: BB
}

//#[Swag.PrintBc]
#test
{
    var x: AA
}