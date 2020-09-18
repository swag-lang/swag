namespace std.algorithms
{
    // Insertion sort algorithm (slow)
    public func(T) sortInsertion(values: [..] T, cb: func(T, T)->s32)
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
    public func(T) sortQuick(values: [..] T, cb: func(T, T)->s32)
    {
        count := @countof(values)
        if(count == 0)
            return

        begin := 0'u32
        end := count - 1
        buffer := @dataof(values)

        while true
        {
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
                tmp := low
                low = high
                high = tmp
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
                    sortQuick(@mkslice(buffer + begin, (low - begin) + 1), cb);
                    begin = high + 1
                }
                else
                {
                    sortQuick(@mkslice(buffer + high + 1, (end - high) + 1), cb);
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
    public func(T) sort(values: [..] T, cb: func(T, T)->s32)
    {
        if(@countof(values) <= 10)
            sortInsertion(values, cb)
        else
            sortQuick(values, cb)
    }
}