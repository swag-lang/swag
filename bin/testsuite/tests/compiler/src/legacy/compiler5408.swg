//#global skip

struct A {}

var g = "666"

impl A
{
    #[Swag.Overload, Swag.Inline]
    mtd const opIndex(i: u64)->const &string
    {
        return &g
    }
}

#[Swag.Optimize(.All, false), Swag.Safety(.All, .All, false)]
#test
{
    var a: A
    let b  = @countof(a[0])
    @assert(b == 3)
}
