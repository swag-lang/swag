using Core

//#[Swag.PrintBc]
#test
{
    var arr: Array'String
    for 10:
        arr.add("toto")

    //Console.write("result is %\n", res)
    var arr1: Array'String
    arr1 = arr

    for 10:
        @assert(arr[cast(u32) @index] == arr1[cast(u32) @index])
    @assert(arr.buffer != arr1.buffer)
    @assert(arr.count == arr1.count)
}
