//#[Swag.PrintBc]
#test
{
    let b   = true
    let x0: f32 = acast b
    @assert(x0 == 1)

    let x1: f64 = acast b
    @assert(x1 == 1)
}
