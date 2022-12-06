struct A
{
    x: [3] {s32}
}

impl A
{
    #[Swag.Inline]
    mtd opIndexAffect(index: uint, value: {s32})
    {
        x[index] = value
    }
}

#test
{
    var a: A
    a[0] = {664}
    @assert(a.x[0].item0 == 664)
}