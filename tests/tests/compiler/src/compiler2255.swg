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

//#[swag.inline]
func toto()->[1] B
{
    var result: [1] B
    return result
}

//#[swag.printbc]
#test
{
    a := toto()
}