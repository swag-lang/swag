


struct(T) Point
{
	x: T
	y: T
}

//#[Swag.PrintBc]
#test
{
	var pt: Point'(f32)
	@assert(pt.x == 0)
	@assert(pt.y == 0)
}