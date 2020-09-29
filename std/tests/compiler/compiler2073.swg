struct AA
{
    val: u64
    val1: u64
}

impl AA
{
    //#[swag.printbc]
    func opPostMove(using self)
    {
    }
}

//#[swag.printbc]
func toto()->AA
{
    var x: AA
    return x
}

//#[swag.printbc]
//#[swag.nooptim]
#test
{
    x, y := toto()
}
