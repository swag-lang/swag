
using core

#test
{
    var arr: Array'u32
    loop i: 10'u32
        arr.add(i)

    result := search.linear(arr.toSlice(), 5)
    @assert(result.success)
    @assert(result.index == 5)

    result1 := search.linear(arr.toSlice(), 15)
    @assert(!result1.success)

    //console.write("result is %\n", res)
}