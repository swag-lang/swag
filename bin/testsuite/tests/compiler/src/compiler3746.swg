interface I
{
    toto: func(self)
}

struct(T) A {}

impl I for A
{
    mtd(N) titi()
    {
        v := @typeof(N)
        @assert(v.name == "s32")
    }

    mtd toto()
    {
        titi's32()
    }
}

#test
{
    var x: A's32
    x.I.toto()
}