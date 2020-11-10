struct Pt
{
    x, y, z: s32
}

//#[swag.printbc]
func toto(x: Pt)
{
    @assert(x.x == 10)
    @assert(x.y == 20)
    @assert(x.z == 30)
}

//#[swag.printbc]
#test
{
    x := 10's32
    y := 20's32
    z := 30's32
    toto(@{x, y, z})
}