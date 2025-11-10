struct TT
{
    x:   s32
}

alias AA = TT

#test
{
    var s = AA{66}
    @assert(s.x == 66)
}
