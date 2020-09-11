#import "std"
using std.collections, std.text, std.diagnostics

#test
{
    var arr1: Array'String
    var arr2: Array'String
    arr1.addAny("0")
    arr1.addAny("1")
    arr2.addAny("2")
    arr2.addAny("3")
    arr1.add(arr2)
    @assert(arr1.count == 4)
    @assert(arr1[2] == "2")
    @assert(arr1[3] == "3")

    var arr3: Array'u8
    //arr3.add(cast(const [..] u8) "toto")
}