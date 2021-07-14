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

//#[Swag.PrintBc]
func toto()->A
{
    var x: A // Should not call opDrop, because we have a,move optim
    return x
}

//#[Swag.PrintBc]
#test
{
    {
        ew := toto()
    }

    @assert(G == 1)
}