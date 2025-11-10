struct A
{
    x:   s32
}

impl A
{
    mtd toto(x: s32)
    {
        me.x = x
    }
}

#test
{
    var v: A
    v.toto(10)
    @assert(v.x == 10)
}
