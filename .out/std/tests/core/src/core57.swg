
using core

//#[Swag.printbc]
#test
{
    var x: BitArray = @[true, false, true, false]

    var y: BitArray
    y.reserve(4, false)

    visit val, idx: x
    {
        y[idx] = val
    }

    @assert(y[0] == true)
    @assert(y[1] == false)
    @assert(y[2] == true)
    @assert(y[3] == false)
}