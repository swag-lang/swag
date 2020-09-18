#import "std"
using std.collections

#test
{
    var x: Array'String
    x.addAny("1")
    x.addAny("2")
    x.addAny("3")
    x.addAny("4")
    x.removeAtOrdered(1)
    @assert(x[0] == "1")
    @assert(x[1] == "3")
    @assert(x[2] == "4")
}