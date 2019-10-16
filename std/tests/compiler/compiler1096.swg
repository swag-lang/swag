#[swag.flags]
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
}
