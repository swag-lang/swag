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
        @assert(false)
    }

    func opPostCopy(using self)
    {
        @assert(false)
    }
}

//#[swag.optim(true)]
func toto()->A
{
    var res: retval
    res.x = 1
    return res
}

//#[swag.printbc]
//#[swag.optim(true)]
func titi()->A
{
    return toto()
}

//#[swag.printbc]
#[swag.optim("", true)]
#test
{
    {
        x := titi()
        @assert(x.x == 1)
        @assert(G == 0)
    }

    @assert(G == 1)
}
