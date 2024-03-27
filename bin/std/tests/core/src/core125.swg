
using Core

#test
{
    var arr: Array'String
    arr.add("0")
    arr.add("1")
    arr.add("2")
    arr.add("3")
    Slice.quickSort(@mkslice(arr.buffer, cast(u64) arr.count), func(x, y) => y <=> x)
    @assert(arr[0] == "3")
    @assert(arr[1] == "2")
    @assert(arr[2] == "1")
    @assert(arr[3] == "0")
}