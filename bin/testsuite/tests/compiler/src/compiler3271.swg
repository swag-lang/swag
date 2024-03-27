func toto(x, y, z: f32 = -1)
{
    @assert(y == 4)
}

//#[Swag.PrintBc]
#test
{
    toto(y: 4)
}
