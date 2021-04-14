#global export
#global namespace slice

// Insertion sort algorithm (slow)
func(T) insertionSort(values: [..] T, cb: func(T, T)->s32)
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
func(T) quickSort(values: [..] T, cb: func(T, T)->s32)
{
    count := @countof(values)
    if(count == 0)
        return

    begin := 0'uint
    end := count - 1
    buffer := @dataof(values)

    while true
    {
        // If range is too small, simple sort
        if end - begin <= 10
        {
            insertionSort(@mkslice(buffer + begin, (end - begin) + 1), cb)
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
        set1 := cast(int) ((low -,safe 1) - begin)
        set2 := cast(int) (end -,safe (high +,safe 1))
        if set1 > 0 && set2 > 0
        {
            if set1 <= set2
            {
                first := begin
                last := low - 1
                quickSort(@mkslice(buffer + first, 1 + (last - first)), cb);
                begin = high + 1
            }
            else
            {
                first := high + 1
                last := end
                quickSort(@mkslice(buffer + first, 1 + (last - first)), cb);
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

// Sort the slice, by picking the right algorithm depending on the type and the
// number of elements
func(T) sort(values: [..] T, cb: func(T, T)->s32 = null)
{
    lambda := cb ?? @(x, y: T)->s32 => x <=> y
    if(@countof(values) <= 10)
        insertionSort(values, lambda)
    else
        quickSort(values, lambda)
}

// Returns true if the slice is sorted
func(T) isSorted(values: const [..] T, cb: func(T, T)->s32 = null)->bool
{
    lambda := cb ?? @(x, y: T)->s32 => x <=> y
    countof := @countof(values)
    loop i: countof - 1
    {
        if lambda(values[i], values[i + 1]) > 0
            return false
    }

    return true
}
