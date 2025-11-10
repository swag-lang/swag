func toto(x, y: s32)->s32
{
    @assert(x == 1)
    @assert(y == 2)
    return 3
}

func toto1(x, y: s32)->s32
{
    @assert(x == 2)
    @assert(y == 3)
    return 5
}

alias T = func(s32, s32)->s32
struct X
{
    t:   [2] T
}

#test
{
    var x: X
    x.t[0] = &toto
    x.t[1] = &toto1

    let res = x.t[0](1, 2)
    @assert(res == 3)

    let res1 = x.t[1](2, 3)
    @assert(res1 == 5)
}
