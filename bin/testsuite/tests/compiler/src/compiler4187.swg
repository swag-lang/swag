//#[Swag.PrintBc]
#[Swag.Safety("sanity", false)]
#[Swag.Optim("", true)]
func toto(z: f32)->f32
{
    const F = 0'f32
    var x = 1
    var y = x + 0'f32
    return z + y
}

#test
{
    @assert(toto(1) == 2.0)
}