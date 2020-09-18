#import "std"
using std

#test
{
    var arr: Array'u32
    loop 100 arr.add(@index)
    sort.quick(@mkslice(arr.buffer, arr.count), @(x, y) => y <=> x)
    loop i: arr
    {
        if i != @countof(arr) - 1
            @assert(arr[i] >= arr[i + 1])
    }
}