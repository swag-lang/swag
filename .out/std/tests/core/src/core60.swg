
using Core

//#[Swag.printbc]
#test
{
    var x: Array's32
    @assert(x.count == 0)
    @assert(x.capacity  == 0)
    @assert(x.buffer == null)
    x.reserve(10)
    @assert(x.capacity  == 10)
}