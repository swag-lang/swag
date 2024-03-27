struct B
{
    x = 0
}

impl B
{
    func opPostMove(using self)
    {
    }
}

//#[Swag.Inline]
func toto()->[1] B
{
    var result: [1] B
    return result
}

//#[Swag.PrintBc]
#test
{
    var a = toto()
}
