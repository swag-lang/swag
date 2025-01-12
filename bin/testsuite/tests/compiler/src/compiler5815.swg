struct(T) Arr
{
    count5815: s32
    buf:   T
}

impl Arr
{
    #[Swag.NoInline]
    mtd opIndex(index: u64)->&T
    {
        return &buf
    }

    #[Swag.NoInline]
    mtd toto()
    {
        @assert(count5815 == 0)
    }
}

#test
{
    var a: Arr'(Arr's32)
    a[0].toto()
}