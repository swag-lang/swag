#test
{
	x := 10000 / cast(f64) 0xFFFFFFFF_FFFFFFFF
	y := 10000 / cast(f64) Swag.U64.Max
	@assert(x == y)
	//@print(cast(f64) x)
}
