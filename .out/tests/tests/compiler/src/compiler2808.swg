// Swap two values
func swap(x, y: *s32)
{
    tmp := move nodrop :x
    :x = nodrop move nodrop :y
    :y = nodrop move nodrop tmp
}

// Quick sort algorithm
func quick(values: [..] s32, cb: func(s32, s32)->s32)
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
        /*@print(cast(s64) low)
        @print(" ")
        @print(cast(s64) begin)
        @print(" ")
        @print(cast(s64) end)
        @print(" ")
        @print(cast(s64) high)
        @print("\n")*/

        //set1 := cast(int) ((low -% 1) - begin)
        //set2 := cast(int) (end -% (high +% 1))
        //if set1 > 0 && set2 > 0
        {
            /*if set1 <= set2
            {
                first := begin
                last := low - 1
                quick(@mkslice(buffer + first, 1 + (last - first)), cb);
                begin = high + 1
            }
            else*/
            {
                first := high + 1
                last := end
                quick(@mkslice(buffer + first, 1 + (last - first)), cb);
                end = low - 1
            }
        }
        /*else if set1 > 0
            end = low - 1
        else if set2 > 0
            begin = high + 1
        else
            break*/
    }
}

#test
{
    var arr: [12] s32
    loop @countof(arr) arr[@index] = cast(s32) @index
    quick(arr, @(x, y) => y <=> x)
}