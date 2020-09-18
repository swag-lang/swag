#import "std"
using std.collections

#test
{
    var arr: Array'u32
    loop 100 arr.add(@index)
    arr.sort(@(x, y) => y <=> x)
    loop i: arr
    {
        if i != @countof(arr) - 1
            @assert(arr[i] >= arr[i + 1])
    }
}