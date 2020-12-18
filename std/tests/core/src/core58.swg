#import "core"
using core

//#[swag.printbc]
#test
{
    var x: BitArray = @[true, false, true, false]

    var y: BitArray
    y.reserve(4, false)

    visit val, idx: x
    {
        y[idx] = val
    }

    @assert(x == y)
}