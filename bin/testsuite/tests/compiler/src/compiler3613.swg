func toto(x, y: s32)
{
    @assert(x == 1)
    @assert(y == 2)
}

#test
{
    var gg: bool
    var t:  [2] func(s32, s32)
    t[0] = &toto
    t[0](1, 2)
}
