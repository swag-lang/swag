#import "std"
using std

//#[swag.printbc]
#test
{
    var arr1: BitArray
    var arr2: BitArray
    arr1.reserve(128, false)
    arr1.set(64, true)
    arr2 = arr1
    @assert(arr1.get(64))
    @assert(arr2.get(64))
}