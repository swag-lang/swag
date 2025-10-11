func toto(x, y: s32)->s32
{
    @assert(x == 1)
    @assert(y == 2)
    return 3
}

#test
{
    var gg: bool
    var t:  [2] func(s32, s32)->s32
    t[0] = &toto
    let x = t[0](1, 2)
    @assert(x == 3)
}
