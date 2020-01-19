struct A
{
}

impl A
{
    func opDrop(using self)
    {
        @assert(false)
    }
}

//#[swag.printbc]
func toto()->A
{
    var x: A // Should not call opDrop, because we have a move optim
    return x 
}

//#[swag.printbc]
#test
{
    toto()
}