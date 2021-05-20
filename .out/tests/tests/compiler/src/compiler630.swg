


struct(T) Point
{
	x: T
	y: T
}

//#[Swag.printbc]
#test
{
	var pt: Point'(f32)
	@assert(pt.x == 0)
	@assert(pt.y == 0)
}