#[swag.printbc]
func(T) sort(values: [..] T, cb: func(T, T)->s32)
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

#test
{
    var x: [] s32 = @[10, 5, 2, 8]
    sort(x, @(x, y) => x - y)
    @assert(x[0] == 2)
    @assert(x[1] == 5)
    @assert(x[2] == 8)
    @assert(x[3] == 10)
}