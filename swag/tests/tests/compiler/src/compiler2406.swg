struct(T, N: u32) NumericArray
{
    buf: [N] T
}

impl NumericArray
{
    func from(values: T...)->Self
    {
        var result: retval
        loop N result.buf[@index] = values[@index]
        return result
    }
}

#test
{
    alias arr4 = NumericArray'(s32, 4)
    x := arr4.from(1, 2, 3, 4)
}