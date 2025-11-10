//#global skip

struct A {}

var g = 4's64

impl A
{
    #[Swag.Overload]
    mtd const opIndex(i: u64)->const &s64
    {
        @assert(i == 4)
        return &g
    }
}

func addr(offset: u64)
{
    @assert(offset == 4)
}

#[Swag.Optimize(.All, false), Swag.Safety(.All, .All, false)]
//#[Swag.PrintBc]
#test
{
    var op: A
    addr(op[4])
}
