
using Core

#test
{
    var x: BitArray
    x.reserve(128, false)
    @assert(x[0] == false)
    x[0] = true
    @assert(x[0] == true)
    @assert(x[1] == false)

    @assert(x[65] == false)
    x[65] = true
    @assert(x[65] == true)
}