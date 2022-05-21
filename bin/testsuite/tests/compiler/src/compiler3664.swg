interface I
{
    toto: func(self)
}

struct X {}
impl I for X
{
    mtd toto() {}
}

//#[Swag.PrintBc]
#test
{
    var a, b: X
    var ai = cast(I) a
    var bi = cast(I) b
    @assert(ai != bi)
}
