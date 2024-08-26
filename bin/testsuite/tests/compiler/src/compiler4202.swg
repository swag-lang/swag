#global testerror "Err0574"
struct B {}

var G = 0

impl B
{
}

struct(T) A
{
    b: T
}

impl A
{
    mtd titi() {}

    #[Swag.Inline]
    mtd toto()
    {
        b.titi()
    }
}

#test
{
    var x: A'B
    x.toto()
    @assert(G == 1)
}