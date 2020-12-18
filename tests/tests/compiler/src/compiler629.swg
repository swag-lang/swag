


struct(T) Point
{
	x: T
	y: T
}

//#[swag.printbc]
#test
{
	var pt: Point'(f32)
	pt.x = 10
	@assert(pt.x == 10.0)
}