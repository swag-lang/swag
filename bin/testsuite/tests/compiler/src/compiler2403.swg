struct(T, N: u32) NumericArray
{
    buf: [N] T
}

impl NumericArray
{
    //#[Swag.PrintBc]
    func set(me, value: T)
    {
        #ast
        {
            @assert(#typeof(T) == f32)
            return "var a = 3.14"
        }

        @assert(a == 3.14)
    }
}

#test
{
    var v: NumericArray'(f32, 4)
    v.set(3.14)
}
