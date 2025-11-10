#[Swag.Optimize(.All, false)]
func(T) toto(x: T)->T
{
    return -x
}

#test
{
    let a0 = toto'f32(-2)
    @assert(a0 == 2)
    let a1 = toto'f64(2)
    @assert(a1 == -2)
}
