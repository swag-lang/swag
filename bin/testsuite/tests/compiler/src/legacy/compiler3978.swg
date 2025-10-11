struct V { x, y, z: f32 }

func toto(v: const &V)
{
    @assert(v.x == 2)
    @assert(v.y == 3)
    @assert(v.z == 4)
}

//#[Swag.PrintBc]
#test
{
    toto({2.0, 3.0, 4})
}
