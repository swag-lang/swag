
using Core

#test
{
    var arr: Array'u32

    loop 10 arr.add(cast(u32) #index)
    arr.insertAt(1, 100)
    @assert(arr[0] == 0)
    @assert(arr[1] == 100)
    @assert(arr[2] == 1)
    @assert(arr[arr.count - 1] == 9)

    @assert(Slice.contains(arr.toSlice(), 100'u32))
    @assert(!Slice.contains(arr.toSlice(), 101'u32))

    var arr1: Array'String
    arr1.add("0")
    arr1.add("1")

    var str: String = "A"
    arr1.insertAt(0, moveref &str)
    @assert(arr1[0] == "A")
    @assert(arr1[1] == "0")
    @assert(arr1[2] == "1")
    @assert(str.isEmpty())
}