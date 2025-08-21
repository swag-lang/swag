struct(T, N: u32) NumericArray
{
    buf: [N] T
}

impl NumericArray
{
    func opIndex(using me, idx: u64)->T
    {
        return buf[idx]
    }
}

#test
{
    var v: NumericArray'(f32, 4)
    var x = v[0]
}