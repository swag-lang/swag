#import "std"
using std

#test
{
    var x: Array'String
    x.reserve(10)
    x.add(String.from("toto"))
    x.add(String.from("titi"))
    @assert(x.count == 2)
    @assert(x.capacity == 10)
}