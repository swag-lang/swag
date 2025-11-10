struct A
{
    x:   [3] { a: s32 }
}

impl A
{
    #[Swag.Inline]
    mtd opIndexAffect(index: u64, value: { a: s32 })
    {
        me.x[index] = value
    }
}

#test
{
    var a: A
    a[0] = {664}
    @assert(a.x[0].item0 == 664)
}
