#test
{
	x := 10000 / cast(f64) 0xFFFFFFFF_FFFFFFFF
	y := 10000 / cast(f64) swag.MaxU64
	@assert(x == y)
	//@print(cast(f64) x)
}
