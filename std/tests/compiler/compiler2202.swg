struct Pt
{
    x, y: s32
}

func toto(x: Pt)
{
    @assert(x.x == 10)
    @assert(x.y == 20)
}

//#[swag.printbc, swag.nooptim]
#test
{
    toto(@{10, 20})
}