


struct(T) Point
{
	x: T
	y: T
}

//#[Swag.PrintBc]
#test
{
	var pt: Point'(f32)
	pt.x = 10
	@assert(pt.x == 10.0)
}