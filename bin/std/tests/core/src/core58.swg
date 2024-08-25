using Core

//#[Swag.PrintBc]
#test
{
    var x: BitArray = [true, false, true, false]

    var y: BitArray
    y.reserve(4, false)

    foreach val, idx in x
    {
        y[idx] = val
    }

    @assert(x == y)
}
