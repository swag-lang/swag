struct A
{
}

var G = 0
impl A
{
    func opDrop(using self)
    {
        G += 1
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
    {
        toto()
    }

    @assert(G == 1)
}