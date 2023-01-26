#test
{
    f := 666
    g := {factor: f}
    @assert(g.factor == 666)

    v := cast(*void) &g

    p := cast(*@mktype(g)) v
    @assert(p.factor == 666)
}