func(T) RR()->{T}
{
    return @{5}
}

#test
{
    res := RR'f64()
    @assert(res.item0 == 5)
    @assert(@typeof(res.item0) == f64)
}
