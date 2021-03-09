
using core

//#[swag.printbc]
#test
{
	x := math.cos(1.0'f32)
	y := math.sqrt(4.0'f32)
	@assert(y == 2)

	x = math.clamp(2'f32, 0, 1)
    //@print(cast(f64) x)
	@assert(cast(f64) x == 1)
}