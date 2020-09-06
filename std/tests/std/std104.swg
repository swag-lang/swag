#import "std"
using std.collections, std.text

#test
{
    var x: Array'String
    x.add(String.from("toto"))
    x.add(String.from("titi"))
    @assert(x.count == 2)
}