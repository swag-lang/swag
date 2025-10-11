struct Pt { x, y, z: s32 }

//#[Swag.PrintBc]
func toto(x: Pt)
{
    @assert(x.x == 10)
    @assert(x.y == 20)
    @assert(x.z == 30)
}

//#[Swag.PrintBc]
#test
{
    let x = 10's32
    let y = 20's32
    let z = 30's32
    toto({x, y, z})
}
