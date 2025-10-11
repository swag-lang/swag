using Core

#test
{
    var arr: Array'u32
    for 100 do
        arr.add(cast(u32) @index)
    Slice.quickSort(@mkslice(arr.buffer, cast(u64) arr.count), func(x, y) => y <=> x)
    for i in arr
    {
        if i != @countof(arr) - 1 do
            @assert(arr[cast(u32) i] >= arr[cast(u32) i + 1])
    }
}
