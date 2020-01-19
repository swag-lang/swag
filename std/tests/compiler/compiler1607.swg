struct A
{
}

impl A
{
    func opDrop(using self)
    {
        @print("opDrop\n")
    }
}

#[swag.printbc]
func toto()->A
{
    var x: A
    return x
}

//#[swag.printbc]
#test
{
    toto()
}