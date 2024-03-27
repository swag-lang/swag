struct C
{
    c: s32
}

struct A
{
}
impl A
{
    mtd opIndex(x: u64)->*C
    {
        return null
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
        titi(v[0].c)
    }
}
