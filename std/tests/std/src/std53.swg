#import "std"
using std

#test
{
    var x: BitArray = @[true, false, true, false]
    var y: BitArray = @[false, true, false, true]

    @assert(x[0] == true)
    @assert(x[1] == false)
    @assert(x[2] == true)
    @assert(x[3] == false)
    x.or(y)
    @assert(x[0] == true)
    @assert(x[1] == true)
    @assert(x[2] == true)
    @assert(x[3] == true)
}