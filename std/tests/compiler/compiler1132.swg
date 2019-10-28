func clamp(x: f32, min: f32, max: f32)->f32
{
	return (x < min) ? (min) : ((x > max) ? (max) : (x));
}

//#[swag.printbc]
#test
{
	x := clamp(2'f32, 0'f32, 1'f32)
	@assert(cast(f64) x == 1)
}