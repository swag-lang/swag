
using Core

//#[Swag.PrintBc]
#test
{
    var arr: Array'String
    loop 10 do arr.add("totototototototototototototototototototototototo")

    var arr1: Array'String
    arr1 = arr

    loop i: 10 do @assert(arr[cast(u32) i] == arr1[cast(u32) i])
    @assert(arr.buffer != arr1.buffer)
    @assert(arr.count == arr1.count)

    //loop 10 Console.write("% ", arr[0])
}