#import "std"
using std.collections, std.text

#test
{
    var x: Array'String
    x.addAny("toto")
    x.addAny("titi")
    @assert(x[0] == "toto")
}