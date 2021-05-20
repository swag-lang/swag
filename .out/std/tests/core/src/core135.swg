
using Core

//#[Swag.printbc]
#test
{
    var arr: Array'String
    loop 10 arr.addAny("toto")

    //Console.write("result is %\n", res)
    var arr1: Array'String
    arr1 = arr

    loop 10 @assert(dref arr[cast(u32) @index] == dref arr1[cast(u32) @index])
    @assert(arr.buffer != arr1.buffer)
    @assert(arr.count == arr1.count)
}