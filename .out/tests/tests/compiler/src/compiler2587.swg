struct AA
{
    x, y: f64
}

impl AA
{
    func opDrop(self)
    {
        a := 0
    }

    func opPostMove(self)
    {
        a := 0
    }
}

//#[Swag.printbc]
func toto()->AA
{
    var result: retval
    return result
}

//#[Swag.printbc]
func titi()->AA
{
    var result: retval = toto()
    return result
}

#test
{
    x := titi()
}