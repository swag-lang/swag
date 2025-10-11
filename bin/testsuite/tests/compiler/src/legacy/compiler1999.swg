func(T) sortQuick(values: [..] T, cb: func(T, T)->s32)
{
    if (@countof(values) == 0) do
        return

    var begin  = 0'u64
    var end    = @countof(values) - 1
    var buffer = @dataof(values)

    for
    {
        // Pick a pivot and #move it out of the way
        let pivot = (begin + end) / 2

        let med = buffer[pivot]
        buffer[pivot] = buffer[end]
        buffer[end]   = med

        var low  = begin
        var high = end
        for
        {
            while low < high and cb(buffer[low], med) <= 0 do
                low += 1
            while low < high and cb(buffer[high], med) >= 0 do
                high -= 1
            if low >= high do
                break
            let tmp = low
            low  = high
            high = tmp
        }

        // Put the median in the "center" of the list
        buffer[end]  = buffer[high]
        buffer[high] = med

        // To avoid too much recursions, we recurse only for the smaller partition
        #[Swag.CanOverflow(true)]
        let set1 = cast(s64) ((low - 1) - begin)
        #[Swag.CanOverflow(true)]
        let set2 = cast(s64) (end - (high + 1))

        if set1 > 0 and set2 > 0
        {
            if set1 <= set2
            {
                sortQuick(@mkslice(buffer + begin, cast(u64) (low - begin) + 1), cb)
                begin = high + 1
            }
            else
            {
                sortQuick(@mkslice(buffer + high + 1, cast(u64) (end - high) + 1), cb)
                end = low - 1
            }
        }
        elif set1 > 0 do
            end = low - 1
        elif set2 > 0 do
            begin = high + 1
        else do
            break
    }
}

#test
{
    var x = [10, 9, 8, 7, 6, 5, 4, 3, 2, 1]
    sortQuick(cast([..] s32) x, func(x, y) => x <=> y)
    for i in x
    {
        if i != @countof(x) - 1 do
            @assert(x[i] <= x[i + 1])
    }

    sortQuick(cast([..] s32) x, func(x, y) => y <=> x)
    for i in x
    {
        if i != @countof(x) - 1 do
            @assert(x[i] >= x[i + 1])
    }
}
