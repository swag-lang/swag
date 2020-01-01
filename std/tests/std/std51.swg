#import "std"
using std

#test
{
    var x: BitArray
    x = [true, false, true, false]
    @assert(x[0] == true)
}