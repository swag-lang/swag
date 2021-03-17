
using core

struct BOUBOU
{
    value: s32 = 666
    value1: s32 = 666
}

var G = 0

impl BOUBOU
{
    func opDrop(using self)
    {
        G += 1
    }
}

//#[swag.printbc]
#test
{
    G = 0
    {
        var arr: Array'BOUBOU
        var value: BOUBOU
        arr.add(value)
    }
    @assert(G == 2)
}