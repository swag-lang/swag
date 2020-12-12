#import "std"
using std

//#[swag.printbc]
#test
{
    var arr: Array'String
    loop 10 arr.addAny("toto")

    //console.write("result is %\n", res)
    var arr1: Array'String
    arr1 = arr

    loop 10 @assert(arr[cast(u32) @index] == arr1[cast(u32) @index])
    @assert(arr.buffer != arr1.buffer)
    @assert(arr.count == arr1.count)
}