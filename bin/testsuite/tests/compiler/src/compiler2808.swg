// Swap two values
func swap(x, y: *s32)
{
    let tmp = #moveraw dref x
    dref x = #nodrop #moveraw dref y
    dref y = #nodrop #moveraw tmp
}

// Quick sort algorithm
func quick(values: [..] s32, cb: func(s32, s32)->s32)
{
    let count = @countof(values)
    if (count == 0) do
        return

    let begin  = 0'u64
    var end    = count - 1
    var buffer = @dataof(values)

    for
    {
        // If range is too small, simple sort
        if end - begin <= 10
        {
            return
        }

        // Pick a pivot and #move it out of the way
        let pivot = (begin + end) / 2
        swap(&buffer[pivot], &buffer[end])

        var low  = begin
        var high = end
        for
        {
            while low < high and cb(buffer[low], buffer[end]) <= 0 do
                low += 1
            while low < high and cb(buffer[high], buffer[end]) >= 0 do
                high -= 1
            if low >= high do
                break
            swap(&buffer[low], &buffer[high])
        }

        // Put the median in the "center" of the list
        swap(&buffer[end], &buffer[high])

        // To avoid too much recursions, we recurse only for the smaller partition
        /*@print(cast(s64) low)
        @print(" ")
        @print(cast(s64) begin)
        @print(" ")
        @print(cast(s64) end)
        @print(" ")
        @print(cast(s64) high)
        @print("\n")*/

        //var set1 = cast(s64) ((low - #wrap 1) - begin)
        //var set2 = cast(s64) (end - #wrap (high + #wrap 1))
        //if set1 > 0 and set2 > 0
        {
            /*if set1 <= set2
            {
                var first = begin
                var last = low - 1
                quick(@mkslice(buffer + first, 1 + (last - first)), cb);
                begin = high + 1
            }
            else*/
            {
                let first = high + 1
                let last  = end
                quick(@mkslice(buffer + first, 1 + (last - first)), cb)
                end = low - 1
            }
        }
        /*elif set1 > 0
            end = low - 1
        elif set2 > 0
            begin = high + 1
        else
            break*/
    }
}

#test
{
    var arr: [12] s32
    for @countof(arr) do
        arr[@index] = cast(s32) @index
    quick(arr, func(x, y) => y <=> x)
}
