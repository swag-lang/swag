#global testerror "Err0166"
struct BB
{
    x:   bool
}

impl BB
{
    mtd opPostMove()
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
