struct A
{
    t: s32 = 2
}

impl A
{
    #[Swag.Inline]
    mtd opIndex(x: u64)->const &s32
    {
        return &t
    }
}

func add(x: s32)
{
    @assert(x == 2)
}

#[Swag.Optim("", false)]
#test
{
    var a: A
    add(a[0])
}