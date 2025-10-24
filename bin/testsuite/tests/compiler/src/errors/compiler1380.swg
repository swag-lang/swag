#global testerror "Err0017"

struct TOTO
{
    x:   s32
}

struct TITI
{
    using base:     TOTO
    x:              s32
}

#test
{
    const p: TITI
    p.x = 666
    @assert(p.base.x == 666)
    @assert(p.x == 666)
}
