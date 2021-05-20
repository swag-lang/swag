


struct(T) Point
{
	x: T = 5
	y: T = 10
}

//#[Swag.printbc]
#test
{
	var pt : Point's32{100, 500}
	@assert(pt.x == 100)
	@assert(pt.y == 500)
}