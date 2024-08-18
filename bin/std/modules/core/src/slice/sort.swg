#global export
#global namespace Slice

// Insertion sort algorithm (slow)
func(T) insertionSort(values: [..] T, cb: closure(T, T)->s32)
{
    var buffer = @dataof(values)
    let count  = @countof(values)

    for var i = 1'u32; i < count; i += 1
    {
        for var j = i; j > 0 and cb(buffer[j], buffer[j - 1]) < 0; j -= 1
        {
            swap(&buffer[j], &buffer[j - 1])
        }
    }
}

// Quick sort algorithm
func(T) quickSort(values: [..] T, cb: closure(T, T)->s32)
{
    let count = cast(s64) @countof(values)
    if count == 0:
        return

    var begin  = 0's64
    var end    = count - 1
    var buffer = @dataof(values)

    loop
    {
        // If range is too small, simple sort
        if end - begin <= 10
        {
            insertionSort(@mkslice(buffer + begin, (end - begin) + 1), cb)
            return
        }

        let pivot = begin
        var low   = begin
        var high  = end
        loop
        {
            while low < high and cb(buffer[low], buffer[pivot]) <= 0:
                low += 1
            while cb(buffer[high], buffer[pivot]) > 0:
                high -= 1
            if low >= high:
                break
            swap(&buffer[low], &buffer[high])
        }

        // Put the median in the "center" of the list
        swap(&buffer[pivot], &buffer[high])

        // To avoid too much recursions, we recurse only for the smaller partition
        let set1 = (high - 1) - begin
        let set2 = end - (high + 1)
        if set1 > 0 and set2 > 0
        {
            var first, last: s64 = undefined
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

            quickSort(@mkslice(buffer + first, (last - first) + 1), cb)
        }
        elif set1 > 0:
            end = high - 1
        elif set2 > 0:
            begin = high + 1
        else:
            break
    }
}

// Sort the slice, by picking the right algorithm depending on the type and the
// number of elements
#[Swag.Overload]
func(T) sort(values: [..] T, bigFirst = false)
{
    if bigFirst:
        sort(values, func(x, y: T)->s32 => y <=> x)
    else:
        sort(values, func(x, y: T)->s32 => x <=> y)
}

// Sort the slice, by picking the right algorithm depending on the type and the
// number of elements
#[Swag.Overload]
func(T) sort(values: [..] T, cb: closure(T, T)->s32)
{
    Debug.assert(cb != null)
    if @countof(values) <= 10:
        insertionSort(values, cb)
    else:
        quickSort(values, cb)
}

// Returns true if the slice is sorted
#[Swag.Overload]
func(T) isSorted(values: const [..] T)->bool
{
    return isSorted(values, func(x, y: T)->s32 => x <=> y)
}

// Returns true if the slice is sorted
#[Swag.Overload]
func(T) isSorted(values: const [..] T, cb: closure(T, T)->s32)->bool
{
    Debug.assert(cb != null)
    let countof = @countof(values)
    loop i in countof - 1
    {
        if cb(values[i], values[i + 1]) > 0:
            return false
    }

    return true
}
