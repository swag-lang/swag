interface I
{
    func toto(me);
}

struct X {}
impl I for X
{
    mtd impl toto() {}
}

//#[Swag.PrintBc]
#test
{
    var a, b: X
    let ai = cast(I) a
    let bi = cast(I) b
    @assert(ai != bi)
}
