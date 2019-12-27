#import "std"
using std

//#[swag.printbc]
#test
{
    var arr: BitArray
    arr.reserve(128)
    @assert(arr.numBits == 128)
    @assert(arr.numBytes == 128 / 8)
    @assert(arr.buffer != null)
    arr.setAll(false)

    @assert(!arr.get(0))
    arr.set(0, true)
    @assert(arr.get(0))

    @assert(!arr.get(48))
    arr.set(48, true)
    @assert(arr.get(48))    
}