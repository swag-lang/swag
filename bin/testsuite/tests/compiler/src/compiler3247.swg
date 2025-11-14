using Swag

struct Color
{
    a, r, g, b: u8
    #[Offset("a")]
    argb: u32
}

impl Color
{
    #[Complete, Inline, ConstExpr]
    func opAffect(me, argb: u32) { me.argb = argb }

    #[ConstExpr]
    func getDarken(col: u32)->u32
    {
        return col
    }
}

struct A
{
    x:   Color = Color.getDarken(666)
}

#test
{
    var v: A
    @assert(v.x.argb == 666)
}
