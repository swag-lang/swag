struct Base
{
    x: s32
}

struct AA
{
    base: Base{x: 666}
}

var v: AA

#test
{
    @assert(v.base.x == 666)
}