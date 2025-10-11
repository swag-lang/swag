//#global skip
using Swag

#[ConstExpr]
struct Color
{
    a, r, g, b: u8
    #[Offset("a")]
    argb: u32
}

impl Color
{
    #[ConstExpr]
    func getDarken()->Color
    {
        var vv: Color
        vv.argb = 666
        return vv
    }
}

struct A
{
    x:   Color = Color.getDarken()
}

#test
{
    var a: A
    @assert(a.x.argb == 666)
}
