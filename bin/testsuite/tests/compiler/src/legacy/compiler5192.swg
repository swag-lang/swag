struct A {}
struct B {}

interface I
{
    mtd toto();
}

impl I for A
{
    mtd impl toto() {}
}

impl I for B
{
    mtd impl toto() {}
}

func toto(y: I, what: typeinfo)->bool
{
    return y == what
}

#test
{
    let a: A
    let b: B

    @assert(toto(cast(I) a, A))
    @assert(toto(cast(I) b, B))
    @assert(!toto(cast(I) a, B))
    @assert(!toto(cast(I) b, A))
}
