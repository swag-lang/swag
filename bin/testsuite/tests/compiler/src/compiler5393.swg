//#global skip
struct A
{
    t:   s32 = 55
}

impl A
{
    #[Swag.Optimize(.All, false)]
    mtd opIndex(x: u64)->const &s32
    {
        return &me.t
    }
}

#[Swag.Optimize(.All, false)]
#test
{
    var a: A
    let b  = cast(s32) a[0]
    @assert(b == 55)
}
