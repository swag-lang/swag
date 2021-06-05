


struct(T) Point
{
	x: T = 6
}

//#[Swag.printbc]
#test
{
	var pt: Point's32
	//pt.opInit()
	@assert(pt.x == 6)

	var pt1: Point'f32
	//pt1.opInit()
	@assert(pt1.x == 6)
}
