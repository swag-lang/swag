struct A
{
    x: s32
}

struct B
{
    using val: A
    v: s32
}

#test
{
    pt := B(v: 555, x: 666's32)
    @assert(pt.x == 666)
    @assert(pt.val.x == 666)
    @assert(pt.v == 555)
}