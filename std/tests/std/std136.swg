#import "std"
using std

//#[swag.printbc]
#test
{
    var arr: Array'String
    loop 10 arr.addAny("totototototototototototototototototototototototo")

    var arr1: Array'String
    arr1 = arr

    loop 10 @assert(arr[@index] == arr1[@index])
    @assert(arr.buffer != arr1.buffer)
    @assert(arr.count == arr1.count)
}