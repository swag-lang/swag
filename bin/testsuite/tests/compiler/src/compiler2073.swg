struct AA
{
    val: u64
    val1: u64
}

impl AA
{
    //#[Swag.PrintBc]
    func opPostMove(using self)
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
    x, y := toto()
}
