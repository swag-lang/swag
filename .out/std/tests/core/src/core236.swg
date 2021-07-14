
using Core

struct AA
{
    ptr: *~64 s32
}

var G = 666

//#[Swag.PrintBc]
#test
{
    var x: AA
    x.ptr = &G

    var arr: Array'AA
    arr.add(x)
    @assert(arr[0].ptr[0] == 666)

    arr.add(x)
    @assert(arr[0].ptr[0] == 666)
    @assert(arr[1].ptr[0] == 666)
}