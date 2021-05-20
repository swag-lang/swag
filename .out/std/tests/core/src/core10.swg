
using Core

//#[Swag.printbc]
#test
{
	x := Math.cos(1.0'f32)
	y := Math.sqrt(4.0'f32)
	@assert(y == 2)

	x = Math.clamp(2'f32, 0, 1)
    //@print(cast(f64) x)
	@assert(cast(f64) x == 1)
}