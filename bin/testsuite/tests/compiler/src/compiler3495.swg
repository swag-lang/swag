struct(T) A
{
    a: s32 = 666
}

impl A
{
    mtd toto()
    {
    }

    mtd toto(x: T)
    {
        @assert(a == 666)
    }
}

struct(T) B
{
    d: f32
    using a: A'(*T)
}

struct C
{
    v: B's32
}

impl C
{
    mtd titi()
    {
        v.toto(4)
    }
}

#test
{
    var c: C
    c.titi()
}