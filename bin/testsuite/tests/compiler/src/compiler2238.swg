struct Pt
{
    x: s64 = 1
}

func toto(x: Pt)
{
    @assert(x.x == 555)
}

#[Swag.ConstExpr]
func offset() => 555

//#[Swag.PrintBc]
#test
{
    toto(Pt{offset()})
}