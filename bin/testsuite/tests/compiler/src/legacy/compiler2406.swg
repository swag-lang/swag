struct(T, N: u32) NumericArray
{
    buf: [N] T
}

impl NumericArray
{
    func from(values: T...)->NumericArray
    {
        var result: retval
        for N do
            result.buf[@index] = values[@index]
        return result
    }
}

#test
{
    alias arr4 = NumericArray'(s32, 4)
    let x = arr4.from(1, 2, 3, 4)
    @assert(x.buf[0] == 1)
    @assert(x.buf[1] == 2)
    @assert(x.buf[2] == 3)
    @assert(x.buf[3] == 4)
}
