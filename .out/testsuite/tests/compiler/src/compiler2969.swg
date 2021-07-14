struct Color
{
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

struct VV
{
    C: Color = 12345678
}

//#[Swag.printbc]
#test
{
    var v: VV
    v.C = 666
    @assert(v.C.argb == 666)
}