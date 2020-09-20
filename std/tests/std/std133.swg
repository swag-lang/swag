#import "std"
using std

#test
{
    var arr: Array'u32
    loop 10 arr.add(@index)
    arr.sort()

    loop 10
        @assert(sort.find(@mkslice(arr.buffer, arr.count), @index) == @index)
    //console.write("result is %\n", res)
}