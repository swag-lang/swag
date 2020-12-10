#import "std"
using std

#test
{
    var arr: Array'u32
    loop 10 arr.add(@index)
    arr.sort()

    loop 10
    {
        result := sort.find(@mkslice(arr.buffer, cast(uint) arr.count), @index)
        @assert(result.success)
        @assert(result.index == @index)
    }

    //console.write("result is %\n", res)
}