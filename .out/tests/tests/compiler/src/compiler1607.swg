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

//#[Swag.printbc]
func toto()->A
{
    var x: A // Should not call opDrop, because we have a,move optim
    return x
}

//#[Swag.printbc]
#test
{
    {
        ew := toto()
    }

    @assert(G == 1)
}