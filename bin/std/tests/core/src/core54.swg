using Core

#test
{
    var x: BitArray = [true, true, true, true]
    var y: BitArray = [false, true, false, true]

    x.xorWith(y)
    @assert(x[0] == true)
    @assert(x[1] == false)
    @assert(x[2] == true)
    @assert(x[3] == false)
}
