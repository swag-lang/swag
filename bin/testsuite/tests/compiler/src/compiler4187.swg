//#[Swag.PrintBc]
#[Swag.Sanity(false), Swag.Optimize(.All, true)]
func toto(z: f32)->f32
{
    const F = 0'f32
    let x   = 1
    let y   = x + 0'f32
    return z + y
}

#test
{
    @assert(toto(1) == 2.0)
}
