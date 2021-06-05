//#[Swag.printbc]
#test
{
    var v: AA
    v.x = 444
    @init(&v)
    @assert(v.x == 666)
}

struct AA
{
    x = 666
}

impl AA
{
    func opDrop(self)
    {
    }

    func opPostCopy(self)
    {
    }

    func opPostMove(self)
    {
    }
}