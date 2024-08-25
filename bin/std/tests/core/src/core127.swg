using Core

#test
{
    var arr: Array'u32
    for 100:
        arr.add(cast(u32) #index)
    arr.sort(func(x, y) => y <=> x)

    for i in arr
    {
        if i != @countof(arr) - 1:
            @assert(arr[cast(u32) i] >= arr[cast(u32) i + 1])
    }

    arr.sort()
    for i in arr
    {
        if i != @countof(arr) - 1:
            @assert(arr[cast(u32) i] <= arr[cast(u32) i + 1])
    }
}
