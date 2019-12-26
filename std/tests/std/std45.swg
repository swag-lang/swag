#import "std"
using std

//#[swag.printbc]
#test
{
    var arr: BitArray
    arr.reserve(128)
    @assert(arr.numBits == 128)
    @assert(arr.numBytes == 128 / 8)
    @print(cast(s64) cast(u64) arr.buffer)
    //arr.setAll(false)
}