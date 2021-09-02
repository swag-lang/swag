struct Base
{
    x: s32 = 10
    y: s32 = 20
}

struct AA
{
    base: Base{x: 666}
}

var v: AA

#test
{
    @assert(v.base.x == 666)
    @assert(v.base.y == 20)
}