struct Color
{
    a, r, g, b: u8
    #[Swag.offset("a")]
    argb: u32
}

impl Color
{
    #[Swag.ConstExpr]
    func opAffect(using self, cl: u32)
    {
        argb = cl
    }
}

var C: Color = 12345678
//const  C: Color = 12345678
#test
{
    @assert(C.argb == 12345678)
}