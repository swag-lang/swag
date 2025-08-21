struct AA
{
    val: u64
    val1: u64
}

impl AA
{
    //#[Swag.PrintBc]
    func opPostMove(using me)
    {
    }
}

//#[Swag.PrintBc]
func toto()->AA
{
    var x: AA
    return x
}

//#[Swag.PrintBc]
//#[Swag.nooptim]
#test
{
    var x, y = toto()
}
