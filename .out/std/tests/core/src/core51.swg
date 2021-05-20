
using core

//#[Swag.printbc]
#test
{
    var x: BitArray = @[true, false, true, false]
    @assert(x[0] == true)
    @assert(x[1] == false)
    @assert(x[2] == true)
    @assert(x[3] == false)
}