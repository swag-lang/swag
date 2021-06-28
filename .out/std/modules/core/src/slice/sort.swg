#global export
#global namespace Slice

// Insertion sort algorithm (slow)
func(T) insertionSort(values: [..] T, cb: func(T, T)->s32)
{
    buffer := @dataof(values)
    count := @countof(values)

    for i := 1'u32; i < count; i += 1
    {
        for j := i; j > 0 and cb(buffer[j], buffer[j - 1]) < 0; j -= 1
        {
            swap(&buffer[j], &buffer[j - 1])
        }
    }
}

// Quick sort algorithm
func(T) quickSort(values: [..] T, cb: func(T, T)->s32)
{
    count := cast(int) @countof(values)
    if(count == 0)
        return

    begin  := 0'int
    end    := count - 1
    buffer := @dataof(values)

    loop
    {
        // If range is too small, simple sort
        /*if end - begin <= 10
        {
            insertionSort(@mkslice(buffer + begin, (end - begin) + 1), cb)
            return
        }*/

        pivot := begin
        low   := begin
        high  := end
        loop
        {
            while low < high and cb(buffer[low], buffer[pivot]) <= 0
                low += 1
            while cb(buffer[high], buffer[pivot]) > 0
                high -= 1
            if low >= high
                break
            swap(&buffer[low], &buffer[high])
        }

        // Put the median in the "center" of the list
        swap(&buffer[pivot], &buffer[high])

        // To avoid too much recursions, we recurse only for the smaller partition
        set1 := (high - 1) - begin
        set2 := end - (high + 1)
        if set1 > 0 and set2 > 0
        {
            var first, last: int = ?
            if set1 <= set2
            {
                first = begin
                last  = high - 1
                begin = high + 1
            }
            else
            {
                first = high + 1
                last  = end
                end   = high - 1
            }

            quickSort(@mkslice(buffer + first, (last - first) + 1), cb);
        }
        else if set1 > 0
            end = high - 1
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
    lambda := cb orelse @(x, y: T)->s32 => x <=> y
    if(@countof(values) <= 10)
        insertionSort(values, lambda)
    else
        quickSort(values, lambda)
}

// Returns true if the slice is sorted
func(T) isSorted(values: const [..] T, cb: func(T, T)->s32 = null)->bool
{
    lambda := cb orelse @(x, y: T)->s32 => x <=> y
    countof := @countof(values)
    loop i: countof - 1
    {
        if lambda(values[i], values[i + 1]) > 0
            return false
    }

    return true
}
