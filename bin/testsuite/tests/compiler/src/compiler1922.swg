struct(T) TT
{
    x: T
}

typealias AA = TT's32

#test
{
    var s = AA{66}
    @assert(s.x == 66)
}