
using core

#test
{
    var arr: Array'u32
    loop 10 arr.add(cast(u32) @index)
    arr.sort()

    loop 10
    {
        result := Slice.findSorted(@mkslice(arr.buffer, arr.count), @index)
        @assert(result.success)
        @assert(result.index == @index)
    }

    //console.write("result is %\n", res)
}