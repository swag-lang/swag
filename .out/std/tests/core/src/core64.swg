
using core

struct X
{
    value: s32 = 666
}

var G = 0

impl X
{
    func opDrop(using self)
    {
        G += 1
        //@print("opDrop64 ")
    }
}

//#[Swag.printbc]
#test
{
    {
        G = 0
        var arr: Array'X
        var value: X
        arr.add(value)
    }

    @assert(G == 2)
}