
using Core

#test
{
    var arr: Array'u32
    loop i: 10'u32
        arr.add(i)

    var result = Slice.findLinear(arr.toSlice(), 5'u32)
    @assert(result.success)
    @assert(result.index == 5)

    var result1 = Slice.findLinear(arr.toSlice(), 15'u32)
    @assert(!result1.success)

    //Console.write("result is %\n", res)
}