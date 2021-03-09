
using core

#test
{
    var x: BitArray = @[true, false, true, false]

    x.not()
    @assert(x[0] == false)
    @assert(x[1] == true)
    @assert(x[2] == false)
    @assert(x[3] == true)
}