struct(T, N: u32) NumericArray
{
    buf: [N] T
}

impl NumericArray
{
    //#[Swag.PrintBc]
    func set(using self, value: T)
    {
        #ast "var a = value"
        @assert(a == 3.14)
    }
}

#test
{
    var v: NumericArray'(f32, 4)
    v.set(3.14)
}