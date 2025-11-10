struct B {}

var G = 0

impl B
{
    mtd titi()
    {
        G = 1
    }
}

struct(T) A
{
    rng: T
}

impl A
{
    mtd titi() {}

    #[Swag.Inline]
    mtd toto()
    {
        #if #defined(T.titi) do
            me.rng.titi()
    }
}

#test
{
    var x: A'B
    x.toto()
    @assert(G == 1)
}
