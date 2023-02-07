
using Core

#test
{
    var x: Array'String
    x.add("1")
    x.add("2")
    x.add("3")
    x.add("4")
    x.removeAtOrdered(1)
    @assert(x[0] == "1")
    @assert(x[1] == "3")
    @assert(x[2] == "4")
}