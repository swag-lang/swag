
using Core

#test
{
    var arr: Array'String
    arr.addAny("0")
    arr.addAny("1")
    arr.addAny("2")
    arr.addAny("3")
    arr.removeAtOrdered(1)
    @assert(dref arr[0] == "0")
    @assert(dref arr[1] == "2")
    @assert(dref arr[2] == "3")
}