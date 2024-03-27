
using Core

#test
{
    var arr: Array'String
    arr.add("0")
    arr.add("1")
    arr.add("2")
    arr.add("3")
    arr.removeAtOrdered(1)
    @assert(arr[0] == "0")
    @assert(arr[1] == "2")
    @assert(arr[2] == "3")
}