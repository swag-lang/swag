#import "core"
using core

#test
{
    var x: Array'String
    x.add(String.from("toto"))
    x.add(String.from("titi"))
    @assert(x.count == 2)
    x.removeAt(0)
    @assert(x.count == 1)
    x.removeAt(0)
    @assert(x.count == 0)
    x.add(String.from("toto"))
    @assert(x.count == 1)
}