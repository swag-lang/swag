struct A
{
    x:   s32 = 666
}

impl A
{
    mtd opIndex(i: u64)->const &s32
    {
        return &me.x
    }
}

//#[Swag.PrintBc]
#test
{
    var a: A
    let b  = &a[0]
    @assert(#typeof(b).kind == .Pointer)
    @assert(dref b == 666)
}
