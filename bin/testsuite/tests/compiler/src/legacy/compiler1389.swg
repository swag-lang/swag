//#[Swag.PrintBc]
#test
{
    let b   = true
    let x0: f32 = cast() b
    @assert(x0 == 1)

    let x1: f64 = cast() b
    @assert(x1 == 1)
}
