struct(T) A
{
    t1:     T
    t:      T = 55
}

impl A
{
    #[Swag.Optimize("", false)]
    mtd opIndex(x: u64)->const &T
    {
        return &me.t
    }
}

#[Swag.Optimize("", false)]
#test
{
    var a: A's32
    var b: s32 = undefined
    b = a[0]
    @assert(b == 55)
}
