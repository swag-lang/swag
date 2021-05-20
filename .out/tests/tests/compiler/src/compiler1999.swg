func(T) sortQuick(values: [..] T, cb: func(T, T)->s32)
{
    if(@countof(values) == 0)
        return

    begin := 0'uint
    end := @countof(values) - 1
    buffer := @dataof(values)

    loop
    {
        // Pick a pivot and,move it out of the way
        pivot := (begin + end) / 2

        med := buffer[pivot]
        buffer[pivot] = buffer[end]
        buffer[end] = med

        low := begin
        high := end
        loop
        {
            while low < high and cb(buffer[low], med) <= 0
                low += 1
            while low < high and cb(buffer[high], med) >= 0
                high -= 1
            if low >= high
                break
            tmp := low
            low = high
            high = tmp
        }

        // Put the median in the "center" of the list
        buffer[end] = buffer[high]
        buffer[high] = med

        // To avoid too much recursions, we recurse only for the smaller partition
        #[Swag.safety("overflow", false)]
        set1 := cast(int) ((low - 1) - begin)
        #[Swag.safety("overflow", false)]
        set2 := cast(int) (end - (high + 1))

        if set1 > 0 and set2 > 0
        {
            if set1 <= set2
            {
                sortQuick(@mkslice(buffer + begin, cast(uint) (low - begin) + 1), cb);
                begin = high + 1
            }
            else
            {
                sortQuick(@mkslice(buffer + high + 1, cast(uint) (end - high) + 1), cb);
                end = low - 1
            }
        }
        else if set1 > 0
            end = low - 1
        else if set2 > 0
            begin = high + 1
        else
            break
    }
}

#test
{
    x := @[10, 9, 8, 7, 6, 5, 4, 3, 2, 1]
    sortQuick(cast([..] s32) x, @(x, y) => x <=> y)
    loop i: x
    {
        if i != @countof(x) - 1
            @assert(x[i] <= x[i + 1])
    }

    sortQuick(cast([..] s32) x, @(x, y) => y <=> x)
    loop i: x
    {
        if i != @countof(x) - 1
            @assert(x[i] >= x[i + 1])
    }
}