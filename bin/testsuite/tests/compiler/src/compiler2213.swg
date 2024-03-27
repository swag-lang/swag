struct A
{
    x = 0's64
}

var G = 0

impl A
{
    func opDrop(using self)
    {
        G += 1
    }

    func opPostMove(using self)
    {
    }

    func opPostCopy(using self)
    {
    }
}

func toto()->A
{
    var res: retval
    res.x = 1
    return res
}

//#[Swag.PrintBc]
#[Swag.Inline]
func titi()->A
{
    return toto()
}

//#[Swag.PrintBc]
#[Swag.Optim("", true)]
#test
{
    {
        var x = titi()
        @assert(x.x == 1)
        @assert(G == 0)
    }

    @assert(G == 1)
}
