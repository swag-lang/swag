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
    //arr.setAll(false)
}