#import "std"
using std

#test
{
    var arr: Array'u32
    loop 10 arr.add(cast(u32) @index)
    arr.sort()

    loop 10
    {
        result := sort.find(@mkslice(arr.buffer, cast(uint) arr.count), cast(u32) @index)
        @assert(result.success)
        @assert(result.index == @index)
    }

    //console.write("result is %\n", res)
}