struct AA
{
    x: u64
}

var G = 0

impl AA
{
    func opDrop(using self)
    {
        G = 1
    }
}

#[Swag.Inline]
func toto()->AA
{
    return AA{};
}

//#[Swag.PrintBc]
#test
{
    {
        ex := toto()
    }
    @assert(G == 1)
}