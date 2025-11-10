func toto1(x, y: s32)
{
    @assert(x == 2)
    @assert(y == 3)
}

alias T = func(s32, s32)
struct X
{
    t:   [2] T
}

#test
{
    var x: X
    x.t[1] = &toto1

    x.t[1](2, 3)
}
