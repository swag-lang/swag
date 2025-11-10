struct Color
{
    a, r, g, b: u8
    #[Swag.Offset("a")]
    argb: u32
}

impl Color
{
    #[Swag.ConstExpr]
    func opAffect(me, cl: u32)
    {
        me.argb = cl
    }
}

var C: Color = 12345678
//const  C: Color = 12345678
#test
{
    @assert(C.argb == 12345678)
}
