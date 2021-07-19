#global testerror
struct BB
{
    x: bool
}

impl BB
{
    method opPostCopy()
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