#global testerror "Err0709"
struct BB
{
    x:   bool
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
    x:   BB
}

//#[Swag.PrintBc]
#test
{
    var x: AA
}
