
#unittest error

struct TOTO
{
    x: s32
}

struct TITI
{
    using base: TOTO
    x: s32
}

#test
{
    let p: TITI
    p.x = 666
    @assert(p.base.x == 666)
    @assert(p.x == 666)
}