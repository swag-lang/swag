struct(T, N: u32) NumericArray
{
    buf: [N] T
}

impl NumericArray
{
    func from(values: T...)->Self
    {
        var result: retval
        for N: result.buf[@index] = values[@index]
        return result
    }
}

#test
{
    alias arr4 = NumericArray'(s32, 4)
    var x = arr4.from(1, 2, 3, 4)
    @assert(x.buf[0] == 1)
    @assert(x.buf[1] == 2)
    @assert(x.buf[2] == 3)
    @assert(x.buf[3] == 4)
}