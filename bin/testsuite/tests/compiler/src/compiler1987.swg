func(T) sort(values: [..] T, cb: func(T, T)->s32)
{
    let buffer = @dataof(values)
    let count  = @countof(values)

    for var i = 1'u32; i < count; i += 1
    {
        let tmp = #move buffer[i]
        var j   = i
        while cb(buffer[j - 1], tmp) > 0
        {
            buffer[j] = #move buffer[j - 1]
            j -= 1
            if j == 0 do
                break
        }

        buffer[j] = #move tmp
    }
}

#test
{
    var x: [?] s32 = [10, 5, 2, 8]
    sort(cast() x, func(x, y) => x - y)
    @assert(x[0] == 2)
    @assert(x[1] == 5)
    @assert(x[2] == 8)
    @assert(x[3] == 10)
}
