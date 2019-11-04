#[swag.constexpr]
func abs(x: f32) => -x

//#[swag.printbc]
#test
{  
    y := -1'f32
    z := abs(y)
    @assert(z == 1)
}