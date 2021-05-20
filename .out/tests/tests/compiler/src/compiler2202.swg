struct Pt
{
    x, y: s32
}

func toto(x: Pt)
{
    @assert(x.x == 10)
    @assert(x.y == 20)
}

//#[Swag.printbc, Swag.nooptim]
#test
{
    toto(@{10, 20})
}