func(T) RR()->{ a: T }
{
    return {5}
}

#test
{
    let res = RR'f64()
    @assert(res.item0 == 5)
    @assert(#typeof(res.item0) == f64)

    let res1 = RR's32()
    @assert(res1.item0 == 5)
    @assert(#typeof(res1.item0) == s32)
}
