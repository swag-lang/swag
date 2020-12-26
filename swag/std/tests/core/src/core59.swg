#import "core"
using core

//#[swag.printbc]
func fromCapa(capa: uint)->BitArray
{
    var x: BitArray
    x.reserve(capa, false)
    return x
}

#test
{
    var x: BitArray = @[true, false, true, false]
    var y: BitArray = fromCapa(4)

    visit val, idx: x
    {
        y[idx] = val
    }

    @assert(x == y)
}