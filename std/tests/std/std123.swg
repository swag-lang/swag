#import "std"
using std.collections, std.text

#test
{
    var arr: Array'String
    arr.addAny("0")
    arr.addAny("1")
    arr.addAny("2")
    arr.addAny("3")
    arr.removeAtOrdered(1)
    @assert(arr[0] == "0")
    @assert(arr[1] == "2")
    @assert(arr[2] == "3")
}