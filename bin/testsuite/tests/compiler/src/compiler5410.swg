//#global skip

struct A {}

var g0 = 4's64
var g1 = -7's64

impl A
{
    #[Swag.Overload, Swag.Inline]
    mtd const opIndex(i: u64)->const &s64
    {
        if i == 0 do
            return &g0
        return &g1
    }
}

//#[Swag.PrintBc]
#test
{
    var a: A
    let b  = a[0] - a[1]
    @assert(b == 11)
}
