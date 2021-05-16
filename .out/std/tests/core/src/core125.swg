
using core

#test
{
    var arr: Array'String
    arr.addAny("0")
    arr.addAny("1")
    arr.addAny("2")
    arr.addAny("3")
    slice.quickSort(@mkslice(arr.buffer, cast(uint) arr.count), @(x, y) => y <=> x)
    @assert(dref arr[0] == "3")
    @assert(dref arr[1] == "2")
    @assert(dref arr[2] == "1")
    @assert(dref arr[3] == "0")
}