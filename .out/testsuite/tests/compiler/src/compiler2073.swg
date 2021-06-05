struct AA
{
    val: u64
    val1: u64
}

impl AA
{
    //#[Swag.printbc]
    func opPostMove(using self)
    {
    }
}

//#[Swag.printbc]
func toto()->AA
{
    var x: AA
    return x
}

//#[Swag.printbc]
//#[Swag.nooptim]
#test
{
    x, y := toto()
}
