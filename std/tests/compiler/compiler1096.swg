#[swag.enumflags]
enum DrawFilters
{
	None
	F2
	F3
}

#test
{
	var x : DrawFilters = DrawFilters.None
	x |= DrawFilters.F2
	@assert(x & DrawFilters.F2 != 0)
	@assert(x & DrawFilters.F2 == DrawFilters.F2)

	//var ptr: *s32
	//ptr += 1.0
}
