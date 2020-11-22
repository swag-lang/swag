//#[swag.printbc]
#test
{
    b := true
    var x0: f32 = autocast b
    @assert(x0 == 1)

    var x1: f64 = autocast b
    @assert(x1 == 1)
}