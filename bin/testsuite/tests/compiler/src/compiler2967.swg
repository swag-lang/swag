struct Color
{
    a, r, g, b: u8
    #[Swag.Offset("a")]
    argb: u32
}

impl Color
{
    #[Swag.ConstExpr]
    func opAffect(using me, cl: u32)
    {
        argb = cl
    }
}

const C: Color = 12345678
#test
{
    #assert(C.argb == 12345678)
}