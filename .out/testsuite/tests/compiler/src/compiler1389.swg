//#[Swag.printbc]
#test
{
    b := true
    var x0: f32 = acast b
    @assert(x0 == 1)

    var x1: f64 = acast b
    @assert(x1 == 1)
}