using Core

#test
{
    var arr: Array'u32
    loop 100 do
        arr.add(cast(u32) #index)
    Slice.quickSort(@mkslice(arr.buffer, cast(u64) arr.count), func(x, y) => y <=> x)
    loop i: arr
    {
        if i != @countof(arr) - 1 do
            @assert(arr[cast(u32) i] >= arr[cast(u32) i + 1])
    }
}
