#import "core"
using core

struct AA
{
    ptr: *~8 s32
}

var G = 666

//#[swag.printbc]
#test
{
    /*var x: AA
    x.ptr = &G

    var arr: Array'AA
    arr.add(x)
    @assert(arr[0].ptr[0] == 666)

    arr.add(x)
    @assert(arr[0].ptr[0] == 666)
    @assert(arr[1].ptr[0] == 666)*/
}