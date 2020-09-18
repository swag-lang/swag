#import "std"
using std.collections, std.text

#test
{
    var x: Array'String
    var y: String = "toto"

    @assert(y.capacity > 0)
    x.emplace(&y)
    @assert(y.capacity == 0)
}