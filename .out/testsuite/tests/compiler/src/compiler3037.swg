#global testerror
struct BB
{
    x: bool
}

impl BB
{
    method opDrop()
    {
        @print("toto")
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