struct TOTO
{
    x: s32
}

struct TITI
{
    using base: TOTO
}

#test
{
    var p: TITI
    p.x = 666
    @assert(p.base.x == 666)
    @assert(p.x == 666)
}