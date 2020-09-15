struct A
{
    x: s32
}

struct B
{
    using val: A
}

#test
{
    pt := B(x: 666's32)
    @assert(pt.x == 666)
    @assert(pt.val.x == 666)
}