
using Core

//#[Swag.PrintBc]
#test
{
    var arr: Array'String
    loop 10 arr.addAny("totototototototototototototototototototototototo")

    var arr1: Array'String
    arr1 = arr

    loop i: 10 @assert(dref arr[cast(u32) i] == dref arr1[cast(u32) i])
    @assert(arr.buffer != arr1.buffer)
    @assert(arr.count == arr1.count)

    //loop 10 Console.write("% ", arr[0])
}