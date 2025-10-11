struct AA
{
    x:   u64
}

var G = 0

impl AA
{
    func opDrop(me)
    {
        G = 1
    }
}

#[Swag.Inline]
func toto()->AA
{
    return AA{}
}

//#[Swag.PrintBc]
#test
{
    {
        var ex = toto()
    }
    @assert(G == 1)
}
