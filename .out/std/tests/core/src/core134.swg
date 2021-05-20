
using core

//#[Swag.printbc]
#test
{
    var arr: Array'u32
    loop 10 arr.add(cast(u32) @index)

    //console.write("result is %\n", res)
    var arr1: Array'u32
    arr1 = arr

    loop 10 @assert(arr[cast(u32) @index] == arr1[cast(u32) @index])
    @assert(arr.buffer != arr1.buffer)
    @assert(arr.count == arr1.count)
}