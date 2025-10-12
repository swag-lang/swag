#[Swag.Optimize(.All, false)]
func(T) toto(x, y: T)->T
{
    return @max(x, y)
}

#test
{
    let a0 = toto'f32(2, 1)
    @assert(a0 == 2)
    let a1 = toto'f64(1, 2)
    @assert(a1 == 2)
}
