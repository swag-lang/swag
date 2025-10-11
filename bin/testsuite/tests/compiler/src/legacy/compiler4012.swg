#test
{
    with var v: { a, b: s32 }
    {
        .item0 = 1
        .item1 = 2
    }
    @assert(v.item0 == 1)
    @assert(v.item1 == 2)
}
