//#[Swag.PrintBc]
#test
{
    var v: AA
    v.x = 444
    @init(&v, 1)
    @assert(v.x == 666)
}

struct AA
{
    x    = 666
}

impl AA
{
    func opDrop(me) {}

    func opPostCopy(me) {}

    func opPostMove(me) {}
}
