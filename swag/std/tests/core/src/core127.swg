
using core

#test
{
    var arr: Array'u32
    loop 100 arr.add(cast(u32) @index)
    arr.sort(@(x, y) => y <=> x)

    loop i: arr
    {
        if i != @countof(arr) - 1
            @assert(arr[cast(u32) i] >= arr[cast(u32) i + 1])
    }

    arr.sort()
    loop i: arr
    {
        if i != @countof(arr) - 1
            @assert(arr[cast(u32) i] <= arr[cast(u32) i + 1])
    }
}