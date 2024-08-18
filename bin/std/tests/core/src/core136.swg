using Core

//#[Swag.PrintBc]
#test
{
    var arr: Array'String
    loop 10:
        arr.add("totototototototototototototototototototototototo")

    var arr1: Array'String
    arr1 = arr

    loop i in 10:
        @assert(arr[cast(u32) i] == arr1[cast(u32) i])
    @assert(arr.buffer != arr1.buffer)
    @assert(arr.count == arr1.count)

    //loop 10 Console.write("% ", arr[0])
}
