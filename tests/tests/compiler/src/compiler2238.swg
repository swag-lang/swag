struct Pt
{
    x: s64 = 1
}

func toto(x: Pt)
{
    @assert(x.x == 555)
}

#[swag.constexpr]
func offset() => 555

//#[swag.printbc]
#test
{
    toto(Pt{offset()})
}