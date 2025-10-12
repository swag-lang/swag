//#global skip

struct(T) A {}

var g = 6

impl A
{
    #[Swag.Overload, Swag.Inline]
    mtd const opIndex(i: u64)->const &T
    {
        return &g
    }

    #[Swag.Overload, Swag.Inline]
    mtd opIndex(i: u64)->&T
    {
        return &g
    }

    mtd toto(x: T) {}
}

#[Swag.Optimize(.All, false), Swag.Safety(.All, .All, false)]
#test
{
    var a: A's32
    a.toto(a[0])
}
