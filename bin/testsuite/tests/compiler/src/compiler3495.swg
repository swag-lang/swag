struct(T) A
{
    a:   s32 = 666
}

impl A
{
    #[Swag.Overload]
    mtd toto() {}

    #[Swag.Overload]
    mtd toto(x: T)
    {
        @assert(me.a == 666)
    }
}

struct(T) B
{
    d:           f32
    using a:     A'(#null *T)
}

struct C
{
    v:   B's32
}

impl C
{
    mtd titi()
    {
        me.v.toto(null)
    }
}

#test
{
    var c: C
    c.titi()
}
