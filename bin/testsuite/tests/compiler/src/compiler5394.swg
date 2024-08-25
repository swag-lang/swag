struct A
{
    t: s32 = 2
}

impl A
{
    #[Swag.Optimize("", false)]
    mtd opIndex(x: u64)->const &s32
    {
        return &t
    }
}

#[Swag.Optimize("", false)]
#test
{
    let C = [1, 2, 4]
    var a: A
    let b = C[a[0]]
    @assert(b == 4)
}