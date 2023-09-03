#global testerror "Err0095"
struct B {}

var G = 0

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
        rng.titi()
    }
}

#test
{
    var x: A'B
    x.toto()
    @assert(G == 1)
}