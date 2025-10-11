struct A
{
    x:   [3] s32
}

impl A
{
    #[Swag.Inline]
    mtd opIndexAffect(index: u64, value: s32)
    {
        me.x[index] = value
    }
}

#test
{
    var a: A
    a[0] = 664
    a[1] = 665
    a[2] = 666
    @assert(a.x[0] == 664)
    @assert(a.x[1] == 665)
    @assert(a.x[2] == 666)
}
