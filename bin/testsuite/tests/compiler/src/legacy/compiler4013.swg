func test()->{ a, b: s32 }
{
    with var v: retval
    {
        .item0 = 1
        .item1 = 2
    }
    @assert(v.item0 == 1)
    @assert(v.item1 == 2)
    return v
}

#test
{
    var x = test()
}
