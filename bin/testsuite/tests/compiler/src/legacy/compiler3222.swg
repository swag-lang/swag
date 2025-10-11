struct A
{
    x:   s32 = 666
}

struct B
{
    a:   A
}

impl B
{
    mtd getA() => &me.a
}

#test
{
    var v:  B
    let val = &v.getA().x
    @assert(dref val == 666)
}
