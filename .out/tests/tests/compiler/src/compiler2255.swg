struct B
{
    x = 0
}

impl B
{
    func opPostMove(using self)
    {
        //@print("postmove")
    }
}

//#[Swag.inline]
func toto()->[1] B
{
    var result: [1] B
    return result
}

//#[Swag.printbc]
#test
{
    a := toto()
}