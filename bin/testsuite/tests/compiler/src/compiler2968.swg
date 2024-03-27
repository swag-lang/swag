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

#test
{
    var v: VV
    @assert(v.C.argb == 12345678)
}