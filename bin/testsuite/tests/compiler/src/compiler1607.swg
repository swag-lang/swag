struct A {}

var G = 0
impl A
{
    func opDrop(me)
    {
        G += 1
    }
}

//#[Swag.PrintBc]
func toto()->A
{
    var x: A // Should not call opDrop, because we have a #move optim
    return x
}

//#[Swag.PrintBc]
#test
{
    {
        var ew = toto()
    }

    @assert(G == 1)
}
