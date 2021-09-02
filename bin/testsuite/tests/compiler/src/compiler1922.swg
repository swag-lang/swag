struct(T) TT
{
    x: T
}

alias AA = TT's32

#test
{
    s := AA{66}
    @assert(s.x == 66)
}