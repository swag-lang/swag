#global testerror "Err0741"
struct BB
{
    x: bool
}

impl BB
{
    mtd opPostCopy()
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