#[Swag.ConstExpr]
struct C { a, b, c, d: s32 }

impl C
{
    #[Swag.Inline, Swag.ConstExpr]
    func from(x: s32)->C
    {
        return {x}
    }
}

struct D
{
    t    = C.from(52)
}

#test
{
    let d: D
    @assert(d.t.a == 52)
}
