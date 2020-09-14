namespace std.algorithms
{
    // Slow sorting (insertion sort algorithm) of a slice. Use it if slice
    // is small, otherwise it's better to use 'sortQuick'
    public func(T) sortInsertion(values: [..] T, cb: func(T, T)->s32)
    {
        buffer := @dataof(values)
        count := @countof(values)

        for i := 1'u32; i < count; i += 1
        {
            tmp := move buffer[i]
            j := i
            while cb(buffer[j - 1], tmp) > 0
            {
                buffer[j] = move buffer[j - 1]
                j -= 1
                if j == 0 break
            }

            buffer[j] = move tmp
        }
    }
}