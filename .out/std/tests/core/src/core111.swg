
using core

#test
{
    var x: Array'String
    x.addAny("1")
    x.addAny("2")
    x.addAny("3")
    x.addAny("4")
    x.removeAtOrdered(1)
    @assert(dref x[0] == "1")
    @assert(dref x[1] == "3")
    @assert(dref x[2] == "4")
}