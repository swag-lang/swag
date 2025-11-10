struct C
{
    c:   s32
}

var gc: C

struct A {}
impl A
{
    mtd opIndex(x: u64)->*C
    {
        return &gc
    }
}

struct B
{
    titi: func(s32)
}

impl B
{
    mtd tata()
    {
        var v: A
        me.titi(v[0].c)
    }
}
