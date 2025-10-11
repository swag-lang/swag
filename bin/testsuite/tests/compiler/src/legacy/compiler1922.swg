struct(T) TT
{
    x:   T
}

alias AA = TT's32

#test
{
    var s = AA{66}
    @assert(s.x == 66)
}
