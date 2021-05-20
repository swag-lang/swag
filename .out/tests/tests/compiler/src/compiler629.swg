


struct(T) Point
{
	x: T
	y: T
}

//#[Swag.printbc]
#test
{
	var pt: Point'(f32)
	pt.x = 10
	@assert(pt.x == 10.0)
}