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

#[Swag.Inline]
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