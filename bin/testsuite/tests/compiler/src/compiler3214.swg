struct A
{
    x:   s32
}

struct B
{
    using base:     A
    xx:             s32
}

struct C
{
    using base: B
}

#test
{
    var v: C
    v.x = 666
    @assert(v.x == 666)
    @assert(v.base.x == 666)
    @assert(v.base.base.x == 666)
}
