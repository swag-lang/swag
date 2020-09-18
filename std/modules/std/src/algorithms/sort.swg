namespace std.sort
{
    // Insertion sort algorithm (slow)
    public func(T) insertion(values: [..] T, cb: func(T, T)->s32)
    {
        buffer := @dataof(values)
        count := @countof(values)

        for i := 1'u32; i < count; i += 1
        {
            for j := i; j > 0 && cb(buffer[j], buffer[j - 1]) < 0; j -= 1
            {
                swap(&buffer[j], &buffer[j - 1])
            }
        }
    }

    // Quick sort algorithm
    public func(T) quick(values: [..] T, cb: func(T, T)->s32)
    {
        count := @countof(values)
        if(count == 0)
            return

        begin := 0'u32
        end := count - 1
        buffer := @dataof(values)

        while true
        {
            // If range is too small, simple sort
            if end - begin <= 10
            {
                insertion(@mkslice(buffer + begin, (end - begin) + 1), cb)
                return
            }

            // Pick a pivot and move it out of the way
            pivot := (begin + end) / 2
            swap(&buffer[pivot], &buffer[end])

            low := begin
            high := end
            while true
            {
                while low < high && cb(buffer[low], buffer[end]) <= 0
                    low += 1
                while low < high && cb(buffer[high], buffer[end]) >= 0
                    high -= 1
                if low >= high
                    break
                swap(&buffer[low], &buffer[high])
            }

            // Put the median in the "center" of the list
            swap(&buffer[end], &buffer[high])

            // To avoid too much recursions, we recurse only for the smaller partition
            set1 := cast(s32) ((low - 1) - begin)
            set2 := cast(s32) (end - (high + 1))
            if set1 > 0 && set2 > 0
            {
                if set1 <= set2
                {
                    first := begin
                    last := low - 1
                    sort.quick(@mkslice(buffer + first, 1 + (last - first)), cb);
                    begin = high + 1
                }
                else
                {
                    first := high + 1
                    last := end
                    sort.quick(@mkslice(buffer + first, 1 + (last - first)), cb);
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

    // Sort, by picking the right algorithm depending on the type and the
    // number of elements
    public func(T) slice(values: [..] T, cb: func(T, T)->s32)
    {
        if(@countof(values) <= 10)
            insertion(values, cb)
        else
            quick(values, cb)
    }

    // Returns true if the slice is sorted
    public func(T) isSorted(values: const [..] T, ascending: bool = true)->bool
    {
        countof := @countof(values)

        if ascending
        {
            loop i: countof - 1
            {
                if values[i] > values[i + 1]
                    return false
            }
        }
        else
        {
            loop i: countof - 1
            {
                if values[i] < values[i + 1]
                    return false
            }
        }

        return true
    }
}