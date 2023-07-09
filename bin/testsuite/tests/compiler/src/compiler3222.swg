struct A
{
    x: s32 = 666
}

struct B
{
    a: A
}

impl B
{
    mtd getA() => &a
}

#test
{
    var v: B
    var val = &v.getA().x
    @assert(dref val == 666)
}