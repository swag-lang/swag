struct(T) A
{
    t1:     T
    t:      T = 55
}

impl A
{
    #[Swag.Optimize(.All, false)]
    mtd opIndex(x: u64)->const &T
    {
        return &me.t
    }
}

#[Swag.Optimize(.All, false)]
#test
{
    var a: A's32
    let b: s32 = a[0]
    @assert(b == 55)
}
