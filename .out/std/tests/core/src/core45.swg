
using core

//#[Swag.printbc]
#test
{
    var arr: BitArray
    arr.reserve(128)
    @assert(arr.count == 128)
    @assert(arr.capacity == 128 / 8)
    @assert(arr.buffer != null)
    arr.setAll(false)

    @assert(!arr.get(0))
    arr.set(0, true)
    @assert(arr.get(0))

    @assert(!arr.get(48))
    arr.set(48, true)
    @assert(arr.get(48))

    arr.invert(48)
    @assert(!arr.get(48))
    arr.invert(48)
    @assert(arr.get(48))
}