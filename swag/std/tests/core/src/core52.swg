
using core

#test
{
    var x: BitArray = @[true, false, true, false]
    var y: BitArray = @[false, true, false, true]

    @assert(x[0] == true)
    @assert(x[1] == false)
    @assert(x[2] == true)
    @assert(x[3] == false)
    x.and(y)
    @assert(x[0] == false)
    @assert(x[1] == false)
    @assert(x[2] == false)
    @assert(x[3] == false)
    @assert(y[0] == false)
    @assert(y[1] == true)
    @assert(y[2] == false)
    @assert(y[3] == true)
}