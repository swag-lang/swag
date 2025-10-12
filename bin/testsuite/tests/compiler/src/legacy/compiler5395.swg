struct A
{
    t:   s32 = 2
}

impl A
{
    #[Swag.Inline]
    mtd opIndex(x: u64)->const &s32
    {
        return &me.t
    }
}

func add(x: s32)
{
    @assert(x == 2)
}

#[Swag.Optimize(.All, false)]
#test
{
    var a: A
    add(a[0])
}
