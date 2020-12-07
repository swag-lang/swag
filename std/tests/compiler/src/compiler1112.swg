#test
{
	x := 10000 / cast(f32) 0xFFFFFFFF
	y := 10000 / cast(f32) swag.MaxU32
	@assert(x == y)
	//@print(cast(f64) x)
}
