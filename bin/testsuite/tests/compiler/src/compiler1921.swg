struct TT
{
    x: s32
}

typealias AA = TT

#test
{
    var s = AA{66}
    @assert(s.x == 66)
}