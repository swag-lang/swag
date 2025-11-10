struct AA { x, y: f64 }

impl AA
{
    func opDrop(me)
    {
        var a = 0
    }

    func opPostMove(me)
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
