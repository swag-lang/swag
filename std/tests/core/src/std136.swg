#import "core"
using core

//#[swag.printbc]
#test
{
    var arr: Array'String
    loop 10 arr.addAny("totototototototototototototototototototototototo")

    var arr1: Array'String
    arr1 = arr

    loop i: 10 @assert(arr[cast(u32) i] == arr1[cast(u32) i])
    @assert(arr.buffer != arr1.buffer)
    @assert(arr.count == arr1.count)

    //loop 10 console.write("% ", arr[0])
}