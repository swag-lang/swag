struct AA
{
    x, y: f64
}

impl AA
{
    func opDrop(self)
    {
        var a = 0
    }

    func opPostMove(self)
    {
        var a = 0
    }
}

//#[Swag.PrintBc]
func toto()->AA
{
    var result: retval
    return result
}

//#[Swag.PrintBc]
func titi()->AA
{
    var result: retval = toto()
    return result
}

#test
{
    var x = titi()
}
