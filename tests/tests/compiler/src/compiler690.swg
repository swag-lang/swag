struct(T) Point
{
	x: T = 5
	y: T = 10
}

//alias Point2f = Point'f32

//#[swag.printbc]
#test
{
	var pt : Point'f32{1.0, 2.0}
	@assert(pt.x == 1)
	@assert(pt.y == 2)
}