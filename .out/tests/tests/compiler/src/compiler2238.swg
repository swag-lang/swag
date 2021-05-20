struct Pt
{
    x: s64 = 1
}

func toto(x: Pt)
{
    @assert(x.x == 555)
}

#[Swag.constexpr]
func offset() => 555

//#[Swag.printbc]
#test
{
    toto(Pt{offset()})
}