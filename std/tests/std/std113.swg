#import "std"
using std.collections, std.text, std.diagnostics

#test
{
    var arr: Array'u32

    loop 10 arr.add(@index)
    arr.insertAt(1, 100)
    @assert(arr[0] == 0)
    @assert(arr[1] == 100)
    @assert(arr[2] == 1)
    @assert(arr[arr.count - 1] == 9)
}