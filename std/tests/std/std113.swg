#import "std"
using std.collections

#test
{
    var arr: Array'u32

    loop 10 arr.add(@index)
    arr.insertAt(1, 100)
    @assert(arr[0] == 0)
    @assert(arr[1] == 100)
    @assert(arr[2] == 1)
    @assert(arr[arr.count - 1] == 9)

    @assert(arr.contains(100))
    @assert(!arr.contains(101))

    var arr1: Array'String
    arr1.addAny("0")
    arr1.addAny("1")

    var str: String = "A"
    arr1.emplaceAt(0, &str)
    @assert(arr1[0] == "A")
    @assert(arr1[1] == "0")
    @assert(arr1[2] == "1")
    @assert(str.isEmpty())
}