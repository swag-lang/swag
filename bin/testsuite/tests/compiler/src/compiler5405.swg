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
}

func toto(a: &A's32, b: &s32)
{
    let c = a[b]
    @assert(c == 6)
}

#[Swag.Optimize(.All, false), Swag.Safety(.All, .All, false)]
#test
{
    var a:  A's32
    var idx = 0
    toto(&a, &idx)
}
