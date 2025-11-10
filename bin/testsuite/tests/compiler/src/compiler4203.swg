struct B {}

impl B
{
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
            @assert(false)
    }
}

#test
{
    var x: A'B
    x.toto()
}
