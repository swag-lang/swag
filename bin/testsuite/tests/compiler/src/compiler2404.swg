struct(T, N: u32) NumericArray
{
    buf: [N] T
}

impl NumericArray
{
    func opIndex(using self, idx: uint)->T
    {
        return buf[idx]
    }
}

#test
{
    var v: NumericArray'(f32, 4)
    x := v[0]
}